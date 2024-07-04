#include "point_light.h"

point_light::point_light(
	event_buses &_buses,
	glm::vec3 _pos,
	light_properties _props,
	attenuation_factors _att_factors
) :
	light(light_type::point, _buses),
	pos(_pos),
	props(_props),
	att_factors(_att_factors)
{}

void point_light::set_uniforms(const std::string &var_name, const shader_program &shader) const {
	shader.set_uniform(var_name + ".type", static_cast<int>(type));
	shader.set_uniform(var_name + ".pos", pos);
	shader.set_uniform(var_name + ".ambient", props.ambient);
	shader.set_uniform(var_name + ".diffuse", props.diffuse);
	shader.set_uniform(var_name + ".specular", props.specular);
	shader.set_uniform(var_name + ".att_c", att_factors.constant);
	shader.set_uniform(var_name + ".att_l", att_factors.linear);
	shader.set_uniform(var_name + ".att_q", att_factors.quadratic);
}
