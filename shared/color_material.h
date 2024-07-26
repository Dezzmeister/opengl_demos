#pragma once
#include "material.h"

// This is just a flat color. An object with this material does not interact with lights.
class color_material : public material {
public:
	const unsigned int color;

	color_material(unsigned int _color);

	void prepare_draw(draw_event &event, const shader_program &shader, render_pass_state &render_pass) const override;

	const std::string& shader_name() const override;

private:
	const static std::string color_shader_name;
	glm::vec3 color_vec;
};

