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

shadow_caster_properties::shadow_caster_properties(
	const glm::vec3 &_dir,
	float _frustum_size,
	float _frustum_far
) :
	eye_pos(glm::vec3(0.0f)),
	dir(_dir),
	view(make_view_mat()),
	proj(glm::ortho(
		-_frustum_size,
		_frustum_size,
		-_frustum_size,
		_frustum_size,
		0.1f,
		_frustum_far
	)),
	view_proj(proj * view)
{}

glm::mat4 shadow_caster_properties::make_view_mat() const {
	const glm::vec3 world_up = get_world_up(-dir);
	const glm::vec3 target = eye_pos + dir;

	return glm::lookAt(eye_pos, target, world_up);
}

void shadow_caster_properties::set_dir(const glm::vec3 &_dir) {
	dir = _dir;
	view = make_view_mat();
	view_proj = proj * view;
}

void shadow_caster_properties::set_eye_pos(const glm::vec3 &_eye_pos) {
	eye_pos = _eye_pos;
	view = make_view_mat();
	view_proj = proj * view;
}

const glm::mat4& shadow_caster_properties::get_mat() const {
	return view_proj;
}

directional_light::directional_light(
	const glm::vec3 _dir,
	const light_properties _props,
	shadow_caster_properties _shadow_props
) :
	light::light(light_type::directional),
	shadow_props(_shadow_props),
	dir(_dir),
	props(_props)
{
	shadow_props.set_dir(dir);
}

void directional_light::prepare_draw(int index, const shader_program &shader, render_pass_state &render_pass) const {
	const int i = (index * light_struct_size);

	shader.set_uniform(lights_loc + i + type_loc, static_cast<int>(type));
	shader.set_uniform(lights_loc + i + dir_loc, dir);
	shader.set_uniform(lights_loc + i + ambient_loc, props.ambient);
	shader.set_uniform(lights_loc + i + diffuse_loc, props.diffuse);
	shader.set_uniform(lights_loc + i + specular_loc, props.specular);

	const int shadow_i = index * shadow_caster_struct_size;

	if (casts_shadow()) {
		shader.set_uniform(shadow_casters_loc + shadow_i + light_space_loc, shadow_props.get_mat());

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

	shader.set_uniform(view_proj_loc, shadow_props.get_mat());
}

void directional_light::set_dir(const glm::vec3 &_dir) {
	dir = _dir;
	shadow_props.set_dir(dir);
}

const glm::vec3& directional_light::get_dir() const {
	return dir;
}

bool directional_light::is_eq(const light &other) const {
	if (type != other.type) {
		return false;
	}

	const directional_light& dl = static_cast<const directional_light&>(other);

	return (dir == dl.dir) && (props == dl.props);
}