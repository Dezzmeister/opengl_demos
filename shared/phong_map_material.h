#pragma once
#include "material.h"
#include "texture.h"
#include "unique_handle.h"

class phong_map_material : public material {
public:
	const std::string diffuse_map_name;
	const std::string specular_map_name;
	float shininess;

	phong_map_material(
		const std::string _diffuse_map_name,
		const std::string _specular_map_name,
		const std::vector<float> _uvs,
		float _shininess
	);

	void create_gl() override;

	void draw(draw_event &event, const shader_program &shader) override;

	const std::string& shader_name() const override;

private:
	const static std::string phong_map_shader_name;

	std::vector<float> uvs;
	unique_handle<unsigned int> vbo;
};
