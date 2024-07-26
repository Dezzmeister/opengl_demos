#include "controllers.h"

key_controller::key_controller(event_buses &_buses, std::vector<short> _watched_keys) :
	event_listener<pre_render_pass_event>(&_buses.render, -20),
	buses(_buses),
	keys({}),
	watched_keys(_watched_keys)
{
	event_listener<pre_render_pass_event>::subscribe();
}

int key_controller::handle(pre_render_pass_event &event) {
	for (const short key : watched_keys) {
		const int is_pressed = glfwGetKey(event.window, key) == GLFW_PRESS;
		const int was_pressed = keys[key];

		if (is_pressed == was_pressed) {
			continue;
		}

		keys[key] = is_pressed;

		if (is_pressed) {
			keydown_event event(key);
			buses.input.fire<keydown_event>(event);
		} else {
			keyup_event event(key);
			buses.input.fire<keyup_event>(event);
		}
	}

	return 0;
}
