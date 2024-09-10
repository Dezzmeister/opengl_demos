#pragma once
#include <chrono>
#include "../shared/events.h"
#include "../shared/text2d.h"

class gui :
	public event_listener<program_start_event>,
	public event_listener<pre_render_pass_event>,
	public event_listener<post_processing_event>
{
public:
	gui(event_buses &_buses);

	int handle(program_start_event &event) override;
	int handle(pre_render_pass_event &event) override;
	int handle(post_processing_event &event) override;

private:
	event_buses &buses;
	const font * debug_font{ nullptr };
	std::chrono::time_point<std::chrono::steady_clock> last_fps_update{};
	uint32_t frames{ 0 };
	float fps{ 0.0f };
};
