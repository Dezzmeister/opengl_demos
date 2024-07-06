#include "directional_light.h"

directional_light::directional_light(const glm::vec3 _dir, const light_properties _props) :
	light::light(light_type::directional),
	dir(_dir),
	props(_props)
{}

void directional_light::prepare_draw(int index, const shader_program &shader) const {
	const int i = (index * light_struct_size);

	shader.set_uniform(lights_loc + i + type_loc, static_cast<int>(type));
	shader.set_uniform(lights_loc + i + dir_loc, dir);
	shader.set_uniform(lights_loc + i + ambient_loc, props.ambient);
	shader.set_uniform(lights_loc + i + diffuse_loc, props.diffuse);
	shader.set_uniform(lights_loc + i + specular_loc, props.specular);
}

bool directional_light::is_eq(const light &other) const {
	if (type != other.type) {
		return false;
	}

	const directional_light& dl = static_cast<const directional_light&>(other);

	return (dir == dl.dir) && (props == dl.props);
}