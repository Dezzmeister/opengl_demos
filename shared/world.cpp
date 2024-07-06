#include "shader_store.h"
#include "world.h"

world::world(event_buses &_buses, std::vector<mesh> _meshes, std::vector<light *> _lights) :
	event_listener<draw_event>(&_buses.render),
	buses(_buses),
	meshes(_meshes),
	lights(_lights),
	meshes_need_sorting(false)
{
	// The meshes are sorted by material first, then by geometry. This allows us to minimize
	// the number of set_uniform and shader use calls per render pass; these are quite costly.
	std::sort(std::begin(meshes), std::end(meshes));

	event_listener<draw_event>::subscribe();
}

int world::handle(draw_event &event) {
	if (meshes.size() == 0) {
		return 0;
	}

	if (meshes_need_sorting) {
		std::sort(std::begin(meshes), std::end(meshes));
		meshes_need_sorting = false;
	}

	material * last_mtl = nullptr;
	geometry * last_geom = nullptr;
	shader_program * curr_shader = nullptr;

	for (int i = ((int)meshes.size()) - 1; i >= 0; i--) {
		mesh &m = meshes[i];

		if (m.mat != last_mtl) {
			last_mtl = m.mat;
			curr_shader = &event.shaders.shaders.at(last_mtl->shader_name());
			curr_shader->use();

			shader_use_event shader_event(*curr_shader);
			buses.render.fire(shader_event);

			last_mtl->prepare_draw(event, *curr_shader);
			prepare_draw_lights(*curr_shader);
		}

		if (m.geom != last_geom) {
			last_geom = m.geom;
			last_geom->prepare_draw();
		}

		assert(("Current shader is not null", curr_shader != nullptr));
		assert(("Current geometry is not null", last_geom != nullptr));

		m.draw(event, *curr_shader);
		last_geom->draw();
	}

	return 0;
}

void world::add_mesh(mesh &m) {
	decltype(meshes)::iterator pos = std::upper_bound(std::begin(meshes), std::end(meshes), m);
	meshes.insert(pos, m);
}

void world::add_mesh_unsorted(mesh &m) {
	meshes.push_back(m);
	meshes_need_sorting = true;
}

void world::remove_mesh(const mesh &m) {
	decltype(meshes)::iterator pos = std::find(std::begin(meshes), std::end(meshes), m);
	meshes.erase(pos, std::next(pos));
}

void world::add_light(light * l) {
	lights.push_back(l);
}

void world::remove_light(const light * l) {
	std::erase(lights, l);
}

void world::prepare_draw_lights(const shader_program &shader) const {
	for (int i = 0; i < lights.size(); i++) {
		const light &l = *lights.at(i);

		l.prepare_draw(i, shader);
	}

	shader.set_uniform(light::num_lights_loc, static_cast<int>(lights.size()));
}