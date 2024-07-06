#pragma once
#include "material.h"

class color_material : public material {
public:
	const unsigned int color;

	color_material(unsigned int _color);

	void prepare_draw(draw_event &event, const shader_program &shader) const override;

	const std::string& shader_name() const override;

private:
	const static std::string color_shader_name;
	glm::vec3 color_vec;
};

