#include "texture_material.h"
#include "texture_store.h"

const std::string texture_material::texture_shader_name("basic_texture");

texture_material::texture_material(std::string _texture_name, std::vector<float> _uvs) :
	texture_name(_texture_name),
	uvs(_uvs),
	vbo(0, [](unsigned int _vbo) {
		glDeleteBuffers(1, &_vbo);
	})
{}

void texture_material::create_gl() {
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);

	// UVS are always at location = 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
}

void texture_material::draw(draw_event &event, const shader_program &shader) {
	const texture &tex = event.textures.textures.at(texture_name);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex.get_id());

	shader.set_uniform("tex", 0);
}

const std::string& texture_material::shader_name() const {
	return texture_material::texture_shader_name;
}
