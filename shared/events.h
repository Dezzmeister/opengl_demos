#pragma once
#include <chrono>
#include <GLFW/glfw3.h>
#include "event.h"
#include "shader_program.h"

// The various "store" classes own common resources of a given type and
// control their lifetimes. These classes load some predefined resources
// on `program_start_event`, and they free these resources on
// `program_stop_event`. These resources are passed to objects on `draw_event`.
class shader_store;
class texture_store;

class light;

// Fires before the game loop starts, but after the GL and GDI contexts are
// created.
struct program_start_event {

};

// Fires after the game loop ends, but before the GL context is destroyed.
struct program_stop_event {
	int reason;

	program_stop_event(int _reason) : reason(_reason) {}
};

struct pre_render_pass_event {
	GLFWwindow * window;
	std::chrono::milliseconds delta;

	pre_render_pass_event(GLFWwindow * _window) : 
		window(_window),
		delta(),
		prev_render_pass(std::chrono::steady_clock::now())
	{}

	int before_fire() {
		std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
		delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev_render_pass);
		prev_render_pass = now;

		return 0;
	}

	int after_fire() {
		return 0;
	}

private:
	std::chrono::time_point<std::chrono::steady_clock> prev_render_pass;
};
static_assert(effectful<pre_render_pass_event>);

struct draw_event {
	GLFWwindow * window;
	shader_store &shaders;
	texture_store &textures;
	glm::mat4 * view;
	// TODO: Make a "world" class, put lights and objects in it
	static constexpr int MAX_LIGHTS = 20;

	draw_event(GLFWwindow * _window, shader_store &_shaders, texture_store &_textures, glm::mat4 * _view) : 
		window(_window),
		shaders(_shaders),
		textures(_textures),
		view(_view),
		lights()
	{}

	int before_fire() {
		return 0;
	}

	int after_fire() {
		lights.clear();

		return 0;
	}

	void add_light(light * l) {
		if (lights.size() == MAX_LIGHTS) {
			// TODO: Figure out what to do about this - maybe exclude some lights
			// based on a heuristic
			throw ("Maximum " + std::to_string(MAX_LIGHTS) + " lights");
		}

		lights.push_back(l);
	}

	light * light_at(size_t pos) const {
		return lights.at(pos);
	}

	size_t num_lights() const {
		return lights.size();
	}

private:
	std::vector<light *> lights;
};
static_assert(effectful<draw_event>);

struct post_render_pass_event {
};

struct shader_use_event {
	const shader_program &shader;

	shader_use_event(const shader_program &_shader) : shader(_shader) {}
};

struct keydown_event {
	const short key;

	keydown_event(const short _key) : key(_key) {}
};

struct keyup_event {
	const short key;

	keyup_event(const short _key) : key(_key) {}
};

using lifecycle_event_bus = event_bus<program_start_event, program_stop_event>;
using render_event_bus = event_bus<pre_render_pass_event, draw_event, post_render_pass_event, shader_use_event>;
using input_event_bus = event_bus<keydown_event, keyup_event>;

struct event_buses {
	lifecycle_event_bus lifecycle;
	render_event_bus render;
	input_event_bus input;
};
