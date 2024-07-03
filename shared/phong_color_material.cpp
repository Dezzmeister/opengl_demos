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
	const static std::string light_name("l");
	static float angle = 0.0f;

	const static glm::vec3 axis(0.0f, 1.0f, 0.0f);
	glm::quat light_rot(cosf(angle / 2), axis * sinf(angle / 2));
	angle += 0.01f;

	glm::vec3 light_pos = light_rot * glm::vec3(1.0f, 0.0f, 0.0f);
	light_pos.x += 5.0f;
	light_pos.z += 5.0f;

	shader.set_uniform("mat.ambient", mat.ambient);
	shader.set_uniform("mat.diffuse", mat.diffuse);
	shader.set_uniform("mat.specular", mat.specular);
	shader.set_uniform("mat.shininess", mat.shininess);

	// TODO: More lights
	if (event.lights.size() == 1) {
		const light * const l = event.lights.at(0);

		if (l->type == light_type::point) {
			const point_light * pl = static_cast<const point_light *>(l);
			pl->set_uniforms(light_name, shader);
		}
	}
}

const std::string& phong_color_material::shader_name() const {
	return phong_color_material::phong_shader_name;
}
