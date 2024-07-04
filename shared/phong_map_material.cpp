#include "phong_map_material.h"
#include "point_light.h"
#include "texture_store.h"

const std::string phong_map_material::phong_map_shader_name("phong_map");

phong_map_material::phong_map_material(
	const std::string _diffuse_map_name,
	const std::string _specular_map_name,
	const std::vector<float> _uvs,
	float _shininess
) :
	diffuse_map_name(_diffuse_map_name),
	specular_map_name(_specular_map_name),
	shininess(_shininess),
	uvs(_uvs),
	vbo(0, [](unsigned int _vbo) {
		glDeleteBuffers(1, &_vbo);
	})
{}

void phong_map_material::create_gl() {
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
}

void phong_map_material::draw(draw_event &event, const shader_program &shader) {
	const texture &diffuse_map = event.textures.textures.at(diffuse_map_name);
	const texture &specular_map = event.textures.textures.at(specular_map_name);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_map.get_id());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular_map.get_id());

	shader.set_uniform("mat.diffuse", 0);
	shader.set_uniform("mat.specular", 1);
	shader.set_uniform("mat.shininess", shininess);

	for (size_t i = 0; i < event.num_lights(); i++) {
		const light * const l = event.light_at(i);

		l->set_uniforms("lights[" + std::to_string(i) + "]", shader);
	}
	shader.set_uniform("num_lights", static_cast<int>(event.num_lights()));
}

const std::string& phong_map_material::shader_name() const {
	return phong_map_material::phong_map_shader_name;
}