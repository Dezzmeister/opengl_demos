#include "directional_light.h"

directional_light::directional_light(event_buses &_buses, const glm::vec3 _dir, const light_properties _props) :
	light::light(light_type::directional, _buses),
	dir(_dir),
	props(_props)
{}

void directional_light::set_uniforms(const std::string &var_name, const shader_program &shader) const {
	shader.set_uniform(var_name + ".type", static_cast<int>(type));
	shader.set_uniform(var_name + ".dir", dir);
	shader.set_uniform(var_name + ".ambient", props.ambient);
	shader.set_uniform(var_name + ".diffuse", props.diffuse);
	shader.set_uniform(var_name + ".specular", props.specular);
}