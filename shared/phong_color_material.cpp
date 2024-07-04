#include "phong_color_material.h"
#include "point_light.h"

phong_color_material_properties::phong_color_material_properties(
	glm::vec3 _ambient,
	glm::vec3 _diffuse,
	glm::vec3 _specular,
	float _shininess
) :
	ambient(_ambient),
	diffuse(_diffuse),
	specular(_specular),
	shininess(_shininess)
{}

const std::string phong_color_material::phong_shader_name("phong_color");

phong_color_material::phong_color_material(phong_color_material_properties &_mat) :
	mat(_mat)
{

}

void phong_color_material::create_gl() {

}

void phong_color_material::draw(draw_event &event, const shader_program &shader) {
	shader.set_uniform("mat.ambient", mat.ambient);
	shader.set_uniform("mat.diffuse", mat.diffuse);
	shader.set_uniform("mat.specular", mat.specular);
	shader.set_uniform("mat.shininess", mat.shininess);

	for (size_t i = 0; i < event.num_lights(); i++) {
		const light * const l = event.light_at(i);

		l->set_uniforms("lights[" + std::to_string(i) + "]", shader);
	}
	shader.set_uniform("num_lights", static_cast<int>(event.num_lights()));
}

const std::string& phong_color_material::shader_name() const {
	return phong_color_material::phong_shader_name;
}
