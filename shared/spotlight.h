#pragma once
#include "light.h"

class spotlight : public light {
public:
	glm::vec3 pos;
	glm::vec3 dir;
	float cos_inner_cutoff;
	float cos_outer_cutoff;
	light_properties props;
	attenuation_factors att_factors;

	spotlight(
		event_buses &_buses,
		const glm::vec3 _pos,
		const glm::vec3 _dir,
		float _inner_cutoff,
		float _outer_cutoff,
		const light_properties _props,
		const attenuation_factors _att_factors
	);

	void set_uniforms(const std::string &var_name, const shader_program &shader) const override;
};
