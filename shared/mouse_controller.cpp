#include <vector>
#include "controllers.h"

namespace {
	std::vector<mouse_controller *> mouse_controllers{};
	bool scroll_callback_bound = false;

	// Unfortunately, scroll input is only accessible through a callback, so we need
	// to do something a little ugly here. I don't know why there would ever be more
	// than one mouse controller, but in case there are, they won't interfere with
	// each other.
	void scroll_callback(GLFWwindow * window, double x_offset, double y_offset) {
		for (mouse_controller * controller : mouse_controllers) {
			controller->set_scroll_offset((float)x_offset, (float)y_offset);
		}
	}
}

mouse_controller::mouse_controller(event_buses &_buses, std::vector<uint8_t> _watched_buttons) :
	event_listener<pre_render_pass_event>(&_buses.render, -20),
	event_listener<program_start_event>(&_buses.lifecycle),
	buses(_buses),
	watched_buttons(_watched_buttons)
{
	event_listener<pre_render_pass_event>::subscribe();
	event_listener<program_start_event>::subscribe();

	mouse_controllers.push_back(this);
}

mouse_controller::~mouse_controller() {
	std::erase(mouse_controllers, this);
}

int mouse_controller::handle(pre_render_pass_event &event) {
	for (uint8_t button : watched_buttons) {
		const int is_pressed = glfwGetMouseButton(event.window, button);
		const int was_pressed = buttons[button];

		if (is_pressed == was_pressed) {
			continue;
		}

		buttons[button] = is_pressed;

		if (is_pressed) {
			mousedown_event event(button);
			buses.input.fire(event);
		} else {
			mouseup_event event(button);
			buses.input.fire(event);
		}
	}

	if (scroll_offset.x != 0.0f || scroll_offset.y != 0.0f) {
		mouse_scroll_event event(scroll_offset);
		buses.input.fire(event);
		scroll_offset = glm::vec2(0.0f);
	}

	return 0;
}

int mouse_controller::handle(program_start_event &event) {
	glfwSetInputMode(event.window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

	if (! scroll_callback_bound) {
		glfwSetScrollCallback(event.window, scroll_callback);
	}

	return 0;
}

void mouse_controller::set_scroll_offset(float x, float y) {
	scroll_offset.x = x;
	scroll_offset.y = y;
}