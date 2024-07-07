#include "phong_map_material.h"
#include "point_light.h"
#include "texture_store.h"
#include "util.h"

static constexpr util::str_kv_pair<int> uniform_locs[] = {
	{ "mat.diffuse", 24 },
	{ "mat.specular", 25 },
	{ "mat.shininess", 26 }
};

const std::string phong_map_material::phong_map_shader_name("phong_map");

phong_map_material::phong_map_material(
	const std::string _diffuse_map_name,
	const std::string _specular_map_name,
	float _shininess
) :
	diffuse_map_name(_diffuse_map_name),
	specular_map_name(_specular_map_name),
	shininess(_shininess)
{}

void phong_map_material::prepare_draw(draw_event &event, const shader_program &shader) const {
	static constexpr int diffuse_loc = util::find_in_map(uniform_locs, "mat.diffuse");
	static constexpr int specular_loc = util::find_in_map(uniform_locs, "mat.specular");
	static constexpr int shininess_loc = util::find_in_map(uniform_locs, "mat.shininess");

	const texture &diffuse_map = event.textures.textures.at(diffuse_map_name);
	const texture &specular_map = event.textures.textures.at(specular_map_name);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_map.get_id());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular_map.get_id());

	shader.set_uniform(diffuse_loc, 0);
	shader.set_uniform(specular_loc, 1);
	shader.set_uniform(shininess_loc, shininess);
}

const std::string& phong_map_material::shader_name() const {
	return phong_map_material::phong_map_shader_name;
}