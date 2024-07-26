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
	GLFWwindow * window;
	shader_store * shaders{ nullptr };
	texture_store * textures{ nullptr };
	int screen_width;
	int screen_height;
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
	glm::mat4 * inv_view;

	draw_event(GLFWwindow * _window, shader_store &_shaders, texture_store &_textures) : 
		window(_window),
		shaders(_shaders),
		textures(_textures),
		view(nullptr),
		inv_view(nullptr)
	{}
};

struct post_render_pass_event {
};

struct screen_resize_event {
	const int old_width;
	const int old_height;
	const int new_width;
	const int new_height;

	screen_resize_event(
		int _old_width,
		int _old_height,
		int _new_width,
		int _new_height
	) : old_width(_old_width),
		old_height(_old_height),
		new_width(_new_width),
		new_height(_new_height)
	{}
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

class player;

struct player_look_event {
	player &p;
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 old_dir;
	const glm::mat4 &view;

	player_look_event(player &_p, glm::vec3 _pos, glm::vec3 _dir, glm::vec3 _old_dir, const glm::mat4 &_view) : 
		p(_p),
		pos(_pos),
		dir(_dir),
		old_dir(_old_dir),
		view(_view)
	{}
};

struct player_move_event {
	player &p;
	glm::vec3 pos;
	glm::vec3 old_pos;
	glm::vec3 dir;

	player_move_event(player &_p, glm::vec3 _pos, glm::vec3 _old_pos, glm::vec3 _dir) :
		p(_p),
		pos(_pos),
		old_pos(_old_pos),
		dir(_dir)
	{}
};

using lifecycle_event_bus = event_bus<program_start_event, program_stop_event>;
using render_event_bus = event_bus<
	pre_render_pass_event,
	draw_event,
	post_render_pass_event,
	shader_use_event,
	screen_resize_event
>;
using input_event_bus = event_bus<keydown_event, keyup_event>;
using player_event_bus = event_bus<player_look_event, player_move_event>;

struct event_buses {
	lifecycle_event_bus lifecycle;
	render_event_bus render;
	input_event_bus input;
	player_event_bus player;
};
