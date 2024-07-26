#include "phong_map_material.h"
#include "point_light.h"
#include "texture_store.h"
#include "shader_constants.h"
#include "util.h"

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

void phong_map_material::prepare_draw(draw_event &event, const shader_program &shader, render_pass_state &render_pass) const {
	static constexpr int diffuse_loc = util::find_in_map(constants::shader_locs, "_texture_mat.diffuse");
	static constexpr int specular_loc = util::find_in_map(constants::shader_locs, "_texture_mat.specular");
	static constexpr int shininess_loc = util::find_in_map(constants::shader_locs, "_texture_mat.shininess");

	const texture &diffuse_map = event.textures.textures.at(diffuse_map_name);
	const texture &specular_map = event.textures.textures.at(specular_map_name);

	const int diffuse_tex_unit = render_pass.used_texture_units++;
	const int specular_tex_unit = render_pass.used_texture_units++;

	glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
	glBindTexture(GL_TEXTURE_2D, diffuse_map.get_id());

	glActiveTexture(GL_TEXTURE0 + specular_tex_unit);
	glBindTexture(GL_TEXTURE_2D, specular_map.get_id());

	shader.set_uniform(diffuse_loc, diffuse_tex_unit);
	shader.set_uniform(specular_loc, specular_tex_unit);
	shader.set_uniform(shininess_loc, shininess);
}

const std::string& phong_map_material::shader_name() const {
	return phong_map_material::phong_map_shader_name;
}