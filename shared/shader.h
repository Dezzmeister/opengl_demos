#pragma once
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "unique_handle.h"
#include "util.h"

enum class shader_type {
	Vertex,
	Fragment
};

template <shader_type ShaderType>
class shader {
public:
	shader(const char * const path, const std::string directives = "\n") :
		id(0, [](unsigned int _handle) {
			glDeleteShader(_handle);
		})
	{
		unsigned int gl_shader_type;

		if (ShaderType == shader_type::Vertex) {
			gl_shader_type = GL_VERTEX_SHADER;
		} else if (ShaderType == shader_type::Fragment) {
			gl_shader_type = GL_FRAGMENT_SHADER;
		} else {
			// This shouldn't be possible
			throw std::invalid_argument("Unsupported shader type");
		}

		std::string shader_code;
		std::ifstream shader_file;

		shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			shader_file.open(path);

			std::stringstream shader_stream;

			shader_stream << shader_file.rdbuf();
			shader_file.close();

			shader_code = shader_stream.str();
		} catch (std::ifstream::failure e) {
			std::cout << "Failed to read shader file: \"" << path << "\"" << std::endl;
			throw e;
		}

		int status;
		const char * const sources[] = {
			"#version 330 core\n",
			directives.c_str(),
			shader_code.c_str()
		};

		id = glCreateShader(gl_shader_type);
		glShaderSource(id, (GLsizei)util::c_arr_size(sources), sources, NULL);
		glCompileShader(id);
		glGetShaderiv(id, GL_COMPILE_STATUS, &status);

		if (!status) {
			char info_log[512];

			glGetShaderInfoLog(id, (GLsizei)util::c_arr_size(info_log), NULL, info_log);
			std::string type = (ShaderType == shader_type::Vertex) ? "vertex" : "fragment";
			std::cout << "Failed to compile " << type << " shader (" << path << "): " << info_log << std::endl;
			// TODO: Proper error classes
			throw "Shader compilation error";
		}
	}

	unsigned int get_id() const {
		if (!id) {
			// TODO: Proper error classes
			throw "Error: shader has been moved";
		}

		return id;
	}

private:
	unique_handle<unsigned int> id;
};

