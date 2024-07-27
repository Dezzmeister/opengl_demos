#include "directional_light.h"
#include "shader_constants.h"
#include "util.h"

static const glm::vec3 y_up(0.0f, 1.0f, 0.0f);
static const glm::vec3 x_up(1.0f, 0.0f, 0.0f);
static const float EPSILON = 1e-4f;

static const glm::vec3& get_world_up(const glm::vec3 &dir) {
	const float len = glm::length(dir);

	if ((len - fabs(glm::dot(dir, y_up))) < EPSILON) {
		return x_up;
	}

	return y_up;
}

directional_light::directional_light(
	const glm::vec3 _dir,
	const light_properties _props,
	const float _pov_frustum_size,
	const float _pov_frustum_far,
	const float _pov_eye_zoom,
	const glm::vec3 _pov_eye_displacement
) :
	light::light(light_type::directional),
	dir(_dir),
	props(_props),
	pov_eye_zoom(_pov_eye_zoom),
	pov_eye_displacement(_pov_eye_displacement),
	pov_proj(glm::ortho(
		-_pov_frustum_size,
		_pov_frustum_size,
		-_pov_frustum_size,
		_pov_frustum_size,
		0.1f,
		_pov_frustum_far
	)),
	pov_view(glm::lookAt(
		(-dir) * pov_eye_zoom + pov_eye_displacement,
		glm::vec3(0.0f),
		get_world_up((-dir) * pov_eye_zoom + pov_eye_displacement)
	)),
	pov_view_proj(pov_proj * pov_view)
{}

void directional_light::prepare_draw(int index, const shader_program &shader, render_pass_state &render_pass) const {
	const int i = (index * light_struct_size);

	shader.set_uniform(lights_loc + i + type_loc, static_cast<int>(type));
	shader.set_uniform(lights_loc + i + dir_loc, dir);
	shader.set_uniform(lights_loc + i + ambient_loc, props.ambient);
	shader.set_uniform(lights_loc + i + diffuse_loc, props.diffuse);
	shader.set_uniform(lights_loc + i + specular_loc, props.specular);

	const int shadow_i = index * shadow_caster_struct_size;

	if (casts_shadow()) {
		shader.set_uniform(shadow_casters_loc + shadow_i + light_space_loc, pov_view_proj);

		const int depth_map_tex_unit = render_pass.used_texture_units++;

		glActiveTexture(GL_TEXTURE0 + depth_map_tex_unit);
		glBindTexture(GL_TEXTURE_2D, depth_map);

		shader.set_uniform(shadow_casters_loc + shadow_i + depth_map_loc, depth_map_tex_unit);
		shader.set_uniform(shadow_casters_loc + shadow_i + shadow_caster_enabled_loc, true);
	} else {
		shader.set_uniform(shadow_casters_loc + shadow_i + shadow_caster_enabled_loc, false);
	}
}

void directional_light::prepare_draw_shadow_map(const shader_program &shader) const {
	static constexpr int view_proj_loc = util::find_in_map(constants::shader_locs, "view_proj");

	shader.set_uniform(view_proj_loc, pov_view_proj);
}

void directional_light::set_dir(const glm::vec3 &_dir) {
	dir = _dir;

	const glm::vec3 scaled_dir = (-dir) * pov_eye_zoom + pov_eye_displacement;

	pov_view = glm::lookAt(
		scaled_dir,
		glm::vec3(0.0f),
		get_world_up(scaled_dir)
	);
	pov_view_proj = pov_proj * pov_view;
}

bool directional_light::is_eq(const light &other) const {
	if (type != other.type) {
		return false;
	}

	const directional_light& dl = static_cast<const directional_light&>(other);

	return (dir == dl.dir) && (props == dl.props);
}