#pragma once
#include <memory>
#include "../shared/events.h"
#include "../shared/mesh.h"
#include "../shared/world.h"
#include "custom_events.h"

class spawn_tool :
	public event_listener<player_look_event>,
	public event_listener<player_move_event>,
	public event_listener<mousedown_event>,
	public event_listener<mouse_scroll_event>,
	public event_listener<keydown_event>,
	public event_listener<pre_render_pass_event>
{
public:
	spawn_tool(
		event_buses &_buses,
		custom_event_bus &_custom_bus,
		short _activation_key,
		geometry * _preview_geom,
		const glm::mat4 &_preview_transform,
		world &_w
	);

	int handle(player_look_event &event) override;
	int handle(player_move_event &event) override;
	int handle(mousedown_event &event) override;
	int handle(mouse_scroll_event &event) override;
	int handle(keydown_event &event) override;
	int handle(pre_render_pass_event &event) override;

private:
	custom_event_bus &custom_bus;
	const short activation_key;
	const glm::mat4 preview_transform;
	std::unique_ptr<mesh> preview;
	world &w;
	bool active{ false };
	glm::vec3 dir{ 0.0f };
	glm::vec3 player_pos{ 0.0f };

	float scroll_f{ 1.0f };
};
