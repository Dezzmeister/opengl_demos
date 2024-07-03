#include <stdio.h>
#include "camera.h"

camera::camera(event_buses &_buses) : 
	event_listener<pre_render_pass_event>(&_buses.render),
	event_listener<shader_use_event>(&_buses.render),
	event_listener<draw_event>(&_buses.render, -100),
	pos(glm::vec3(0.0f, 0.0f, 0.0f)),
	dir(glm::vec3(0.0f, 0.0f, -1.0f)),
	world_up(glm::vec3(0.0f, 1.0f, 0.0f)),
	right(glm::normalize(glm::cross(world_up, dir))),
	camera_up(glm::cross(dir, right)),
	target(glm::vec3(0.0f, 0.0f, 1.0f)),
	view(glm::mat4(1.0f)),
	projection(glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f))
{
	event_listener<pre_render_pass_event>::subscribe();
	event_listener<shader_use_event>::subscribe();
	event_listener<draw_event>::subscribe();
}

int camera::handle(pre_render_pass_event &event) {
	right = glm::normalize(glm::cross(world_up, dir));
	camera_up = glm::cross(dir, right);
	target = pos + dir;
	view = glm::lookAt(pos, target, world_up);

	return 0;
}

int camera::handle(shader_use_event &event) {
	event.shader.set_uniform("view", view);
	event.shader.set_uniform("projection", projection);

	return 0;
}

int camera::handle(draw_event &event) {
	event.view = &view;

	return 0;
}

void camera::update_right_vec() {
	right = glm::normalize(glm::cross(world_up, dir));
}
