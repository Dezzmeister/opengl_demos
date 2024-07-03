#include "point_light.h"

point_light::point_light(event_buses &_buses, glm::vec3 _pos, light_properties _props) :
	light(light_type::point, _buses),
	pos(_pos),
	props(_props)
{}

void point_light::set_uniforms(const std::string &var_name, const shader_program &shader) const {
	shader.set_uniform(var_name + ".pos", pos);
	shader.set_uniform(var_name + ".ambient", props.ambient);
	shader.set_uniform(var_name + ".diffuse", props.diffuse);
	shader.set_uniform(var_name + ".specular", props.specular);
}
