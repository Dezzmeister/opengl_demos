#include "hardware_constants.h"
#include "shader_store.h"
#include "world.h"

template <typename T>
static bool deref_cmp(const T * a, const T * b) {
	return *a < *b;
}

world::world(event_buses &_buses, std::vector<mesh *> _meshes, std::vector<light *> _lights) :
	event_listener<pre_render_pass_event>(&_buses.render),
	event_listener<draw_event>(&_buses.render),
	event_listener<screen_resize_event>(&_buses.render),
	event_listener<program_start_event>(&_buses.lifecycle),
	buses(_buses),
	meshes(_meshes),
	lights(_lights),
	meshes_need_sorting(false),
	instanced_meshes{},
	particle_emitters{},
	screen_width(0),
	screen_height(0),
	max_tex_units(-1),
	default_sampler2d_tex_unit(-1),
	default_cubesampler_tex_unit(-1)
{
	// The meshes are sorted by material first, then by geometry. This allows us to minimize
	// the number of set_uniform and shader use calls per render pass; these are quite costly.
	std::sort(std::begin(meshes), std::end(meshes));

	event_listener<pre_render_pass_event>::subscribe();
	event_listener<draw_event>::subscribe();
	event_listener<screen_resize_event>::subscribe();
	event_listener<program_start_event>::subscribe();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

int world::handle(program_start_event &event) {
	screen_width = event.screen_width;
	screen_height = event.screen_height;
	default_sampler2d_tex_unit = event.hardware_consts->get_max_texture_units() - 1;
	default_cubesampler_tex_unit = event.hardware_consts->get_max_texture_units() - 2;
	max_tex_units = default_cubesampler_tex_unit;

	return 0;
}

int world::handle(screen_resize_event &event) {
	screen_width = event.new_width;
	screen_height = event.new_height;

	return 0;
}

int world::handle(pre_render_pass_event &event) {
	long long millis = event.delta.count();

	for (int64_t i = particle_emitters.size() - 1; i >= 0; i--) {
		particle_emitters.at(i)->update((float)millis);

		if (particle_emitters.at(i)->is_done()) {
			particle_emitters.erase(std::begin(particle_emitters) + i);
		}
	}

	return 0;
}

int world::handle(draw_event &event) {
	if (meshes_need_sorting) {
		std::sort(std::begin(meshes), std::end(meshes), deref_cmp<mesh>);
		meshes_need_sorting = false;
	}

	prepare_shadow_maps(event);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screen_width, screen_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_instanced_meshes(event);

	const material * last_mtl = nullptr;
	const geometry * last_geom = nullptr;
	shader_program * curr_shader = nullptr;
	render_pass_state render_pass(
		default_sampler2d_tex_unit,
		default_cubesampler_tex_unit,
		max_tex_units
	);

	for (const mesh * m : meshes) {
		if (m->mat != last_mtl) {
			render_pass.reset();
			last_mtl = m->mat;
			curr_shader = &event.shaders.shaders.at(last_mtl->shader_name());
			curr_shader->use();

			shader_use_event shader_event(*curr_shader);
			buses.render.fire(shader_event);

			last_mtl->prepare_draw(event, *curr_shader, render_pass);
			prepare_draw_lights(*curr_shader, render_pass);
		}

		if (m->geom != last_geom) {
			last_geom = m->geom;
			last_geom->prepare_draw();
		}

		assert(("Current shader is not null", curr_shader != nullptr));
		assert(("Current geometry is not null", last_geom != nullptr));

		m->prepare_draw(event, *curr_shader, true);
		m->draw();
	}

	curr_shader = nullptr;

	for (const particle_emitter * pe : particle_emitters) {
		shader_program * next_shader = &event.shaders.shaders.at(pe->shader_name());

		if (next_shader != curr_shader) {
			curr_shader = next_shader;
			curr_shader->use();

			shader_use_event shader_event(*curr_shader);
			buses.render.fire(shader_event);
		}

		pe->prepare_draw(event, *curr_shader);
		pe->draw();
	}

	return 0;
}

void world::draw_instanced_meshes(draw_event &event) const {
	if (instanced_meshes.size() == 0) {
		return;
	}

	render_pass_state render_pass(
		default_sampler2d_tex_unit,
		default_cubesampler_tex_unit,
		max_tex_units
	);

	for (const instanced_mesh * im : instanced_meshes) {
		render_pass.reset();

		shader_program &shader = event.shaders.shaders.at(im->mtl->shader_name() + "_instanced");
		shader.use();

		shader_use_event shader_event(shader);
		buses.render.fire(shader_event);

		im->mtl->prepare_draw(event, shader, render_pass);
		prepare_draw_lights(shader, render_pass);

		im->draw(event, shader);
	}
}

void world::prepare_shadow_maps(draw_event &event) const {
	int i = 0;
	for (; i < lights.size(); i++) {
		if (lights[i]->casts_shadow()) {
			break;
		}
	}

	if (i == lights.size()) {
		return;
	}

	glCullFace(GL_FRONT);

	for (; i < lights.size(); i++) {
		const light * l = lights.at(i);

		if (!l->casts_shadow()) {
			continue;
		}

		l->prepare_shadow_render_pass();

		if (instanced_meshes.size()) {
			const shader_program &shadow_shader_instanced = event.shaders.shaders.at(l->shadow_map_shader_name() + "_instanced");

			shadow_shader_instanced.use();

			shader_use_event instanced_shader_event(shadow_shader_instanced);
			buses.render.fire(instanced_shader_event);

			l->prepare_draw_shadow_map(shadow_shader_instanced);

			for (const instanced_mesh * im : instanced_meshes) {
				im->draw(event, shadow_shader_instanced);
			}
		}

		if (meshes.size()) {
			const geometry * last_geom = nullptr;
			const shader_program &shadow_shader = event.shaders.shaders.at(l->shadow_map_shader_name());
			shadow_shader.use();

			shader_use_event shader_event(shadow_shader);
			buses.render.fire(shader_event);

			l->prepare_draw_shadow_map(shadow_shader);

			for (const mesh * m : meshes) {
				if (m->geom != last_geom) {
					last_geom = m->geom;
					last_geom->prepare_draw();
				}

				m->prepare_draw(event, shadow_shader, false);

				assert(("Current geometry is not null (shadow map loop)", last_geom != nullptr));

				m->draw();
			}
		}
	}

	glCullFace(GL_BACK);
}

void world::add_mesh(mesh * m) {
	decltype(meshes)::iterator pos = std::upper_bound(std::begin(meshes), std::end(meshes), m, deref_cmp<mesh>);
	meshes.insert(pos, m);
}

void world::add_mesh_unsorted(mesh * m) {
	meshes.push_back(m);
	meshes_need_sorting = true;
}

void world::remove_mesh(const mesh * m) {
	decltype(meshes)::iterator pos = std::lower_bound(std::begin(meshes), std::end(meshes), m, deref_cmp<mesh>);

	if (**pos != *m) {
		return;
	}

	meshes.erase(pos, std::next(pos));
}

void world::add_light(light * l) {
	lights.push_back(l);
}

void world::remove_light(const light * l) {
	std::erase(lights, l);
}

void world::add_instanced_mesh(instanced_mesh * _mesh) {
	instanced_meshes.push_back(std::move(_mesh));
}

void world::remove_instanced_mesh(const instanced_mesh * _mesh) {
	std::erase_if(instanced_meshes, [_mesh](const instanced_mesh * a) {
		return *a == *_mesh;
	});
}

void world::add_particle_emitter(particle_emitter * emitter) {
	particle_emitters.push_back(emitter);
}

void world::remove_particle_emitter(particle_emitter * emitter) {
	std::erase(particle_emitters, emitter);
}

void world::prepare_draw_lights(const shader_program &shader, render_pass_state &render_pass) const {
	int i = 0;
	for (; i < lights.size(); i++) {
		const light &l = *lights.at(i);

		l.prepare_draw(i, shader, render_pass);
	}

	shader.set_uniform(light::num_lights_loc, static_cast<int>(lights.size()));

	for (; i < light::max_lights; i++) {
		static constexpr int depth_map_loc = util::find_in_map(constants::shader_locs, "shadow_caster.depth_map");
		static constexpr int cubemap_loc = util::find_in_map(constants::shader_locs, "shadow_caster.cube_depth_map");
		static constexpr int shadow_caster_size = util::find_in_map(constants::shader_locs, "sizeof(shadow_caster)");
		static constexpr int shadow_casters_loc = util::find_in_map(constants::shader_locs, "shadow_casters");

		const int shadow_i = i * shadow_caster_size;

		shader.set_uniform(shadow_casters_loc + shadow_i + depth_map_loc, 31);
		shader.set_uniform(shadow_casters_loc + shadow_i + cubemap_loc, 30);
	}
}

const std::vector<light *>& world::get_lights() const {
	return lights;
}