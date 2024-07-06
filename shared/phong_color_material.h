#pragma once
#include "material.h"

struct phong_color_material_properties {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	phong_color_material_properties() = default;

	phong_color_material_properties(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _shininess);
};

class phong_color_material : public material {
public:
	phong_color_material(phong_color_material_properties &_mat);

	const std::string& shader_name() const override;

	void prepare_draw(draw_event &event, const shader_program &shader) const override;

private:
	const static std::string phong_shader_name;

	const phong_color_material_properties &mat;
};

