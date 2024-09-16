#include "../shared/draw2d.h"
#include "../shared/phong_color_material.h"
#include "constants.h"
#include "particle_utils.h"
#include "pointer_tool.h"

namespace {
	void write_vec3(std::stringstream &ss, const std::string &name, const phys::vec3 &v) {
		ss << name << ": (" << v.x << ", " << v.y << ", " << v.z << ")\n";
	};

	void write_real(std::stringstream &ss, const std::string &name, const phys::real r) {
		ss << name << ": " << r << "\n";
	}
}

pointer_tool::pointer_tool(
	event_buses &_buses,
	custom_event_bus &_custom_bus,
	texture_store &_textures,
	world &_mesh_world
) :
	tool(
		_textures.store("pointer_tool_icon", texture("./icons/empty-hand.png")),
		"Pointer Tool",
		"Point at a particle to see its physical\nproperties."
	),
	event_listener<program_start_event>(&_buses.lifecycle),
	event_listener<pre_render_pass_event>(&_buses.render),
	event_listener<post_processing_event>(&_buses.render),
	event_listener<particle_select_event>(&_custom_bus),
	event_listener<particle_deselect_event>(&_custom_bus),
	mesh_world(_mesh_world),
	selected_particle_mesh(std::make_unique<mesh>(sphere_geom.get(), &selected_sphere_mtl))
{
	event_listener<program_start_event>::subscribe();
	event_listener<particle_select_event>::subscribe();
	event_listener<particle_deselect_event>::subscribe();
}

void pointer_tool::activate() {
	if (selected_particle) {
		mesh_world.add_mesh(selected_particle_mesh.get());
	}

	event_listener<pre_render_pass_event>::subscribe();
	event_listener<post_processing_event>::subscribe();
}

void pointer_tool::deactivate() {
	if (selected_particle) {
		mesh_world.remove_mesh(selected_particle_mesh.get());
	}

	event_listener<pre_render_pass_event>::unsubscribe();
	event_listener<post_processing_event>::unsubscribe();
}

bool pointer_tool::is_active() const {
	return event_listener<pre_render_pass_event>::is_subscribed();
}

int pointer_tool::handle(program_start_event &event) {
	debug_font = &event.draw2d->get_font("spleen_6x12");

	return 0;
}

int pointer_tool::handle(pre_render_pass_event &event) {
	if (! selected_particle) {
		return 0;
	}

	update_meshes();

	return 0;
}

int pointer_tool::handle(post_processing_event &event) {
	if (! selected_particle) {
		return 0;
	}

	int width = 256;
	int height = 100;
	int sw = 3;

	event.draw2d.draw_rect(
		0,
		event.screen_height - height,
		width,
		height,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	event.draw2d.draw_rect(
		sw,
		event.screen_height - height + sw,
		width - (2 * sw),
		height - (2 * sw),
		glm::vec4(1.0f, 1.0f, 0.917f, 1.0f)
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
		sw + 2,
		event.screen_height - height + sw + debug_font->glyph_height,
		width - (2 * sw) + 2,
		height - (2 * sw),
		0,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f)
	);

	return 0;
}

int pointer_tool::handle(particle_select_event &event) {
	selected_particle = &event.p;
	particle_index = event.particle_index;
	particle_mesh = &event.particle_mesh;

	move_mesh_to_particle(selected_particle_mesh.get(), selected_particle);

	if (is_active()) {
		mesh_world.add_mesh(selected_particle_mesh.get());
	}

	return 0;
}

int pointer_tool::handle(particle_deselect_event &event) {
	selected_particle = nullptr;

	if (is_active()) {
		mesh_world.remove_mesh(selected_particle_mesh.get());
	}

	return 0;
}

void pointer_tool::update_meshes() {
	move_mesh_to_particle(selected_particle_mesh.get(), selected_particle);
	hide_particle(particle_mesh, particle_index);
}