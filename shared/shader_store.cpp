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
	shaders.insert(std::make_pair("basic_color", shader_program(
		shader<shader_type::Vertex>("../resources/basic_color_vert.glsl"),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/basic_color_frag.glsl")
	)));
	shaders.insert(std::make_pair("basic_texture", shader_program(
		shader<shader_type::Vertex>("../resources/basic_texture_vert.glsl"),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/basic_texture_frag.glsl")
	)));
	shaders.insert(std::make_pair("phong_color", shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", max_lights_define),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", max_lights_define)
	)));
	shaders.insert(std::make_pair("phong_map", shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define USE_MAPS\n" + max_lights_define),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", "#define USE_MAPS\n" + max_lights_define)
	)));
	shaders.insert(std::make_pair("phong_color_instanced", shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define INSTANCED\n" + max_lights_define),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", max_lights_define)
	)));
	shaders.insert(std::make_pair("phong_map_instanced", shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define USE_MAPS\n#define INSTANCED\n" + max_lights_define),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", "#define USE_MAPS\n" + max_lights_define)
	)));
	shaders.insert(std::make_pair("phong_color_transparent", shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define TRANSPARENCY\n" + max_lights_define),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", "#define TRANSPARENCY\n" + max_lights_define)
	)));
	shaders.insert(std::make_pair("phong_map_transparency", shader_program(
		shader<shader_type::Vertex>("../resources/phong_vert.glsl", "#define USE_MAPS\n#define TRANSPARENCY\n" + max_lights_define),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/phong_frag.glsl", "#define USE_MAPS\n#define TRANSPARENCY\n" + max_lights_define)
	)));
	shaders.insert(std::make_pair("shadow_map", shader_program(
		shader<shader_type::Vertex>("../resources/shadow_vert.glsl"),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/identity_frag.glsl")
	)));
	shaders.insert(std::make_pair("shadow_map_instanced", shader_program(
		shader<shader_type::Vertex>("../resources/shadow_vert.glsl", "#define INSTANCED\n"),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/identity_frag.glsl")
	)));
	shaders.insert(std::make_pair("tex_sampler", shader_program(
		shader<shader_type::Vertex>("../resources/tex_sampler_vert.glsl"),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/tex_sampler_frag.glsl")
	)));
	shaders.insert(std::make_pair("cube_sampler", shader_program(
		shader<shader_type::Vertex>("../resources/tex_sampler_vert.glsl"),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/cube_sampler_frag.glsl")
	)));
	shaders.insert(std::make_pair("point_shadow_map", shader_program(
		shader<shader_type::Vertex>("../resources/point_shadow_vert.glsl"),
		shader<shader_type::Geometry>("../resources/point_shadow_geom.glsl"),
		shader<shader_type::Fragment>("../resources/point_shadow_frag.glsl")
	)));
	shaders.insert(std::make_pair("point_shadow_map_instanced", shader_program(
		shader<shader_type::Vertex>("../resources/point_shadow_vert.glsl", "#define INSTANCED\n"),
		shader<shader_type::Geometry>("../resources/point_shadow_geom.glsl", "#define INSTANCED\n"),
		shader<shader_type::Fragment>("../resources/point_shadow_frag.glsl", "#define INSTANCED\n")
	)));
	shaders.insert(std::make_pair("particle_color", shader_program(
		shader<shader_type::Vertex>("../resources/particle_color_vert.glsl"),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/particle_color_frag.glsl")
	)));
	shaders.insert(std::make_pair("text2d", shader_program(
		shader<shader_type::Vertex>("../resources/text2d_vert.glsl"),
		shader<shader_type::Geometry>("../resources/text2d_geom.glsl"),
		shader<shader_type::Fragment>("../resources/text2d_frag.glsl")
	)));
	shaders.insert(std::make_pair("rect2d", shader_program(
		shader<shader_type::Vertex>("../resources/rect2d_vert.glsl"),
		std::nullopt,
		shader<shader_type::Fragment>("../resources/rect2d_frag.glsl")
	)));

	event.shaders = this;

	return 0;
}

int shader_store::handle(program_stop_event &event) {
	shaders.clear();

	return 0;
}
