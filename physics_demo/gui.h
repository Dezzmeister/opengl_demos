#pragma once
#include <chrono>
#include "../shared/events.h"
#include "../shared/physics/particle.h"
#include "../shared/text2d.h"
#include "custom_events.h"

class gui :
	public event_listener<program_start_event>,
	public event_listener<pre_render_pass_event>,
	public event_listener<post_processing_event>,
	public event_listener<particle_select_event>,
	public event_listener<particle_deselect_event>
{
public:
	gui(event_buses &_buses, custom_event_bus &_custom_bus);

	int handle(program_start_event &event) override;
	int handle(pre_render_pass_event &event) override;
	int handle(post_processing_event &event) override;
	int handle(particle_select_event &event) override;
	int handle(particle_deselect_event &event) override;

private:
	event_buses &buses;
	const font * debug_font{ nullptr };
	std::chrono::time_point<std::chrono::steady_clock> last_fps_update{};
	uint32_t frames{ 0 };
	float fps{ 0.0f };
	const phys::particle * selected_particle{ nullptr };
	size_t particle_index{ 0 };

	void draw_fps_count(const post_processing_event &event) const;
	void draw_crosshair(const post_processing_event &event) const;
	void draw_particle_info(const post_processing_event &event) const;
};
