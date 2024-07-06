#pragma once
#include "light.h"

class point_light : public light {
public:
	glm::vec3 pos;
	light_properties props;
	attenuation_factors att_factors;

	point_light(glm::vec3 _pos, light_properties _props, attenuation_factors _att_factors);

	void prepare_draw(int index, const shader_program &shader) const override;

protected:

	bool is_eq(const light &other) const override;
};

