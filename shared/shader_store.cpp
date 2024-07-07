#include "shader_store.h"

shader_store::shader_store(event_buses &_buses) :
	event_listener<program_start_event>(&_buses.lifecycle, -100),
	event_listener<program_stop_event>(&_buses.lifecycle),
	shaders()
{
	event_listener<program_start_event>::subscribe();
	event_listener<program_stop_event>::subscribe();
}

int shader_store::handle(program_start_event &event) {
	shaders.insert(std::make_pair(std::string("basic_color"), shader_program(
		shader<shader_type::Vertex>("../resources/basic_color_vert.glsl"),
		shader<shader_type::Fragment>("../resources/basic_color_frag.glsl")
	)));
	shaders.insert(std::make_pair(std::string("basic_texture"), shader_program(
		shader<shader_type::Vertex>("../resources/basic_texture_vert.glsl"),
		shader<shader_type::Fragment>("../resources/basic_texture_frag.glsl")
	)));
	shaders.insert(std::make_pair(std::string("phong_color"), shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl"),
		shader<shader_type::Fragment>("../resources/phong_frag.glsl")
	)));
	shaders.insert(std::make_pair(std::string("phong_map"), shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define USE_MAPS\n"),
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", "#define USE_MAPS\n")
	)));
	shaders.insert(std::make_pair(std::string("phong_color_instanced"), shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define INSTANCED\n"),
		shader<shader_type::Fragment>("../resources/phong_frag.glsl")
	)));
	shaders.insert(std::make_pair(std::string("phong_map_instanced"), shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define USE_MAPS\n#define INSTANCED\n"),
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", "#define USE_MAPS\n")
	)));

	event.shaders = this;

	return 0;
}

int shader_store::handle(program_stop_event &event) {
	shaders.clear();

	return 0;
}
