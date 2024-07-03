#pragma once
#include <vector>
#include "material.h"
#include "unique_handle.h"

class texture_material : public material {
public:
	const std::string texture_name;

	texture_material(std::string _texture_name, std::vector<float> _uvs);

	void create_gl() override;

	void draw(draw_event &event, const shader_program &shader) override;

	const std::string& shader_name() const override;

private:
	const static std::string texture_shader_name;

	std::vector<float> uvs;
	unique_handle<unsigned int> vbo;
};

