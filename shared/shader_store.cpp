#include "light.h"
#include "shader_store.h"

static const std::string max_lights_define = "#define MAX_LIGHTS\t" + std::to_string(light::max_lights) + "\n";

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
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", max_lights_define),
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", max_lights_define)
	)));
	shaders.insert(std::make_pair(std::string("phong_map"), shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define USE_MAPS\n" + max_lights_define),
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", "#define USE_MAPS\n" + max_lights_define)
	)));
	shaders.insert(std::make_pair(std::string("phong_color_instanced"), shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define INSTANCED\n" + max_lights_define),
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", max_lights_define)
	)));
	shaders.insert(std::make_pair(std::string("phong_map_instanced"), shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define USE_MAPS\n#define INSTANCED\n" + max_lights_define),
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", "#define USE_MAPS\n" + max_lights_define)
	)));
	shaders.insert(std::make_pair(std::string("shadow_map"), shader_program(
		shader<shader_type::Vertex>("../resources/shadow_vert.glsl"),
		shader<shader_type::Fragment>("../resources/identity_frag.glsl")
	)));
	shaders.insert(std::make_pair(std::string("shadow_map_instanced"), shader_program(
		shader<shader_type::Vertex>("../resources/shadow_vert.glsl", "#define INSTANCED\n"),
		shader<shader_type::Fragment>("../resources/identity_frag.glsl")
	)));
	shaders.insert(std::make_pair(std::string("tex_sampler"), shader_program(
		shader<shader_type::Vertex>("../resources/tex_sampler_vert.glsl"),
		shader<shader_type::Fragment>("../resources/tex_sampler_frag.glsl")
	)));

	event.shaders = this;

	return 0;
}

int shader_store::handle(program_stop_event &event) {
	shaders.clear();

	return 0;
}
