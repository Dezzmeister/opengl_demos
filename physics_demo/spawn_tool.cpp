#include "../shared/phong_color_material.h"
#include "spawn_tool.h"

namespace {
	const float min_scroll_f = 0.2f;
	const float max_scroll_f = 10.0f;
	const float scroll_f_step = 0.1f;

	phong_color_material preview_mtl{
		phong_color_material_properties{
			glm::vec3(0.0215, 0.1745, 0.0215),
			glm::vec3(0.07568, 0.61424, 0.07568),
			glm::vec3(0.633, 0.727811, 0.633),
			128 * 0.6f
		}
	};
}

spawn_tool::spawn_tool(
	event_buses &_buses,
	custom_event_bus &_custom_bus,
	short _activation_key,
	geometry * _preview_geom,
	const glm::mat4 &_preview_transform,
	world &_w
) :
	event_listener<player_look_event>(&_buses.player),
	event_listener<player_move_event>(&_buses.player),
	event_listener<mousedown_event>(&_buses.input),
	event_listener<mouse_scroll_event>(&_buses.input),
	event_listener<keydown_event>(&_buses.input),
	event_listener<pre_render_pass_event>(&_buses.render),
	custom_bus(_custom_bus),
	activation_key(_activation_key),
	preview_transform(_preview_transform),
	preview(std::make_unique<mesh>(_preview_geom, &preview_mtl)),
	w(_w)
{
	event_listener<player_look_event>::subscribe();
	event_listener<player_move_event>::subscribe();
	event_listener<mousedown_event>::subscribe();
	event_listener<mouse_scroll_event>::subscribe();
	event_listener<keydown_event>::subscribe();
	event_listener<pre_render_pass_event>::subscribe();
}

int spawn_tool::handle(player_look_event &event) {
	dir = event.dir;

	return 0;
}

int spawn_tool::handle(player_move_event &event) {
	player_pos = event.pos;

	return 0;
}

int spawn_tool::handle(mousedown_event &event) {
	if (! active) {
		return 0;
	}

	sphere_spawned_event spawn_event(player_pos + (dir * scroll_f));
	custom_bus.fire(spawn_event);

	return 0;
}

int spawn_tool::handle(mouse_scroll_event &event) {
	if (! active) {
		return 0;
	}

	scroll_f += event.offset.y * scroll_f_step;
	scroll_f = std::clamp(scroll_f, min_scroll_f, max_scroll_f);

	return 0;
}

int spawn_tool::handle(keydown_event &event) {
	if (event.key == activation_key) {
		active = ! active;

		if (active) {
			w.add_mesh(preview.get());
		} else {
			w.remove_mesh(preview.get());
		}
	}

	return 0;
}

int spawn_tool::handle(pre_render_pass_event &event) {
	if (! active) {
		return 0;
	}

	const glm::mat4 trans_part = glm::translate(
		glm::identity<glm::mat4>(),
		player_pos + (dir * scroll_f)
	);

	preview->set_model(trans_part * preview_transform);

	return 0;
}