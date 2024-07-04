#pragma once
#include "light.h"

class point_light : public light {
public:
	glm::vec3 pos;
	light_properties props;
	attenuation_factors att_factors;

	point_light(event_buses &_buses, glm::vec3 _pos, light_properties _props, attenuation_factors _att_factors);

	void set_uniforms(const std::string &var_name, const shader_program &shader) const override;
};

