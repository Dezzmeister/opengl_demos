#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <type_traits>
#include <unordered_map>
#include "shader.h"

class shader_program {
public:

	shader_program(shader<shader_type::Vertex> vertex_shader, shader<shader_type::Fragment> fragment_shader);

	void use() const;

	unsigned int get_id() const;

	void set_uniform(const std::string name, float value) const;
	void set_uniform(const std::string name, int value) const;
	void set_uniform(const std::string name, unsigned int value) const;
	void set_uniform(const std::string name, const glm::mat4 &value) const;
	void set_uniform(const std::string name, const glm::vec3 &value) const;
	void set_uniform(const std::string name, const glm::mat3 &value) const;

private:
	unique_handle<unsigned int> id;
	mutable std::unordered_map<std::string, int> uniforms;

	int get_location(std::string name) const;
};
