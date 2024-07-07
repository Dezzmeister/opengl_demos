#include "phong_color_material.h"
#include "point_light.h"
#include "util.h"

static constexpr util::str_kv_pair<int> uniform_locs[] = {
	{ "mat.ambient", 24 },
	{ "mat.diffuse", 25 },
	{ "mat.specular", 26 },
	{ "mat.shininess", 27 }
};

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

const std::string& phong_color_material::shader_name() const {
	return phong_color_material::phong_shader_name;
}

void phong_color_material::prepare_draw(draw_event &event, const shader_program &shader) const {
	static constexpr int ambient_loc = util::find_in_map(uniform_locs, "mat.ambient");
	static constexpr int diffuse_loc = util::find_in_map(uniform_locs, "mat.diffuse");
	static constexpr int specular_loc = util::find_in_map(uniform_locs, "mat.specular");
	static constexpr int shininess_loc = util::find_in_map(uniform_locs, "mat.shininess");

	shader.set_uniform(ambient_loc, mat.ambient);
	shader.set_uniform(diffuse_loc, mat.diffuse);
	shader.set_uniform(specular_loc, mat.specular);
	shader.set_uniform(shininess_loc, mat.shininess);
}
