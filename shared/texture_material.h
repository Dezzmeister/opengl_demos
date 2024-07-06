#pragma once
#include <vector>
#include "material.h"

class texture_material : public material {
public:
	const std::string texture_name;

	texture_material(std::string _texture_name);

	void prepare_draw(draw_event &event, const shader_program &shader) const override;

	const std::string& shader_name() const override;

private:
	const static std::string texture_shader_name;
};

