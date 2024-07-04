#include "spotlight.h"

spotlight::spotlight(
	event_buses &_buses,
	const glm::vec3 _pos,
	const glm::vec3 _dir,
	float _inner_cutoff,
	float _outer_cutoff,
	const light_properties _props,
	const attenuation_factors _att_factors
) :
	light::light(light_type::spot, _buses),
	pos(_pos),
	dir(_dir),
	cos_inner_cutoff(cosf(_inner_cutoff)),
	cos_outer_cutoff(cosf(_outer_cutoff)),
	props(_props),
	att_factors(_att_factors)
{}

void spotlight::set_uniforms(const std::string &var_name, const shader_program &shader) const {
	shader.set_uniform(var_name + ".type", static_cast<int>(type));
	shader.set_uniform(var_name + ".pos", pos);
	shader.set_uniform(var_name + ".dir", dir);
	shader.set_uniform(var_name + ".ambient", props.ambient);
	shader.set_uniform(var_name + ".diffuse", props.diffuse);
	shader.set_uniform(var_name + ".specular", props.specular);
	shader.set_uniform(var_name + ".inner_cutoff", cos_inner_cutoff);
	shader.set_uniform(var_name + ".outer_cutoff", cos_outer_cutoff);
	shader.set_uniform(var_name + ".att_c", att_factors.constant);
	shader.set_uniform(var_name + ".att_l", att_factors.linear);
	shader.set_uniform(var_name + ".att_q", att_factors.quadratic);
}
