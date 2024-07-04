#pragma once
#include "light.h"

class directional_light : public light {
public:
	glm::vec3 dir;
	light_properties props;

	directional_light(event_buses &_buses, const glm::vec3 _dir, const light_properties _props);

	void set_uniforms(const std::string &var_name, const shader_program &shader) const override;
};
