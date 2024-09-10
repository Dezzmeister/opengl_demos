#include <iomanip>
#include "gui.h"
#include "../shared/shader_store.h"
#include "../shared/draw2d.h"
#include "../shared/texture_store.h"

using namespace std::literals::chrono_literals;

namespace {
	void write_vec3(std::stringstream &ss, const std::string &name, const phys::vec3 &v) {
		ss << name << ": (" << v.x << ", " << v.y << ", " << v.z << ")\n";
	};

	void write_real(std::stringstream &ss, const std::string &name, const phys::real r) {
		ss << name << ": " << r << "\n";
	}
}

gui::gui(event_buses &_buses, custom_event_bus &_custom_bus) :
	event_listener<program_start_event>(&_buses.lifecycle),
	event_listener<pre_render_pass_event>(&_buses.render),
	event_listener<post_processing_event>(&_buses.render),
	event_listener<particle_select_event>(&_custom_bus),
	event_listener<particle_deselect_event>(&_custom_bus),
	buses(_buses)
{
	event_listener<program_start_event>::subscribe();
	event_listener<pre_render_pass_event>::subscribe();
	event_listener<post_processing_event>::subscribe();
	event_listener<particle_select_event>::subscribe();
	event_listener<particle_deselect_event>::subscribe();
}

int gui::handle(program_start_event &event) {
	debug_font = &event.draw2d->get_font("spleen_6x12");

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
	draw_crosshair(event);
	draw_fps_count(event);
	draw_particle_info(event);

	return 0;
}

int gui::handle(particle_select_event &event) {
	selected_particle = &event.p;
	particle_index = event.particle_index;

	return 0;
}

int gui::handle(particle_deselect_event &event) {
	selected_particle = nullptr;

	return 0;
}

void gui::draw_fps_count(const post_processing_event &event) const {
	std::stringstream ss{};
	ss << std::setprecision(4) << " FPS: " << fps;

	event.draw2d.draw_text(
		ss.str(),
		*debug_font,
		0, 12,
		0, 0, 0,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.4f)
	);
}

void gui::draw_crosshair(const post_processing_event &event) const {
	event.draw2d.draw_text(
		"o",
		*debug_font,
		(event.screen_width - debug_font->glyph_width) / 2,
		(event.screen_height + debug_font->glyph_height) / 2,
		0, 0, 0,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f)
	);
}

void gui::draw_particle_info(const post_processing_event &event) const {
	if (! selected_particle) {
		return;
	}

	event.draw2d.draw_rect(
		0,
		event.screen_height - (6 * debug_font->glyph_height),
		40 * debug_font->glyph_width,
		6 * debug_font->glyph_height,
		glm::vec4(0.4f)
	);

	std::stringstream ss{};
	ss << std::setprecision(5);

	write_vec3(ss, "force", selected_particle->force);
	write_vec3(ss, "pos", selected_particle->pos);
	write_vec3(ss, "vel", selected_particle->vel);
	write_vec3(ss, "acc", selected_particle->acc);
	write_real(ss, "mass", selected_particle->get_mass());
	write_real(ss, "radius", selected_particle->radius);

	event.draw2d.draw_text(
		ss.str(),
		*debug_font,
		0,
		(event.screen_height - (5 * debug_font->glyph_height)),
		0, 0, 0,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f)
	);
}
