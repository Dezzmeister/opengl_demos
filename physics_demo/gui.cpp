#include <iomanip>
#include "gui.h"
#include "../shared/shader_store.h"
#include "../shared/text2d.h"
#include "../shared/texture_store.h"

using namespace std::literals::chrono_literals;

gui::gui(event_buses &_buses) :
	event_listener<program_start_event>(&_buses.lifecycle),
	event_listener<pre_render_pass_event>(&_buses.render),
	event_listener<post_processing_event>(&_buses.render),
	buses(_buses)
{
	event_listener<program_start_event>::subscribe();
	event_listener<pre_render_pass_event>::subscribe();
	event_listener<post_processing_event>::subscribe();
}

int gui::handle(program_start_event &event) {
	debug_font = &event.text2d->get_font("spleen_6x12");

	return 0;
}

int gui::handle(pre_render_pass_event &event) {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	if (now - last_fps_update > 1s) {
		long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_fps_update).count();

		fps = ((float)millis * frames) / 1000.0f;

		last_fps_update = now;
		frames = 0;
	}

	frames++;

	return 0;
}

int gui::handle(post_processing_event &event) {
	std::stringstream ss{};
	ss << std::setprecision(4) << " FPS: " << fps;

	event.text2d.draw_text(
		ss.str(),
		*debug_font,
		0,
		12,
		0,
		0,
		0,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.4f)
	);

	return 0;
}
