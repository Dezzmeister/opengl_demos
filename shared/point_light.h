#pragma once
#include "light.h"

class point_light : public light {
public:
	glm::vec3 pos;
	light_properties props;

	point_light(event_buses &_buses, glm::vec3 _pos, light_properties _props);

	virtual void set_uniforms(const std::string &var_name, const shader_program &shader) const;
};

