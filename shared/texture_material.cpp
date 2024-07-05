#include "texture_material.h"
#include "texture_store.h"

const std::string texture_material::texture_shader_name("basic_texture");

texture_material::texture_material(std::string _texture_name) :
	texture_name(_texture_name)
{}

void texture_material::draw(draw_event &event, const shader_program &shader) {
	const texture &tex = event.textures.textures.at(texture_name);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex.get_id());

	shader.set_uniform("tex", 0);
}

const std::string& texture_material::shader_name() const {
	return texture_material::texture_shader_name;
}
