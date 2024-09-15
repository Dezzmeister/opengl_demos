#include "../shared/texture_store.h"
#include "constants.h"
#include "particle_utils.h"
#include "connector_spawn_tool.h"

connector_spawn_tool::connector_spawn_tool(
	event_buses &_buses,
	custom_event_bus &_custom_bus,
	texture_store &_textures,
	world &_mesh_world
) :
	tool(_textures.store("rod_spawn_tool_icon", texture("./icons/new-rod.png"))),
	event_listener<pre_render_pass_event>(&_buses.render),
	event_listener<mousedown_event>(&_buses.input),
	event_listener<particle_select_event>(&_custom_bus),
	event_listener<particle_deselect_event>(&_custom_bus),
	mesh_world(_mesh_world),
	buses(_buses),
	custom_bus(_custom_bus),
	selected_a_mesh(std::make_unique<mesh>(sphere_geom.get(), &selected_sphere_mtl))
{
	event_listener<particle_select_event>::subscribe();
	event_listener<particle_deselect_event>::subscribe();
}

void connector_spawn_tool::activate() {
	event_listener<pre_render_pass_event>::subscribe();
	event_listener<mousedown_event>::subscribe();
}

void connector_spawn_tool::deactivate() {
	particle_a_index = -1;
	particle_b_index = -1;

	if (particle_a) {
		mesh_world.remove_mesh(selected_a_mesh.get());
	}

	event_listener<pre_render_pass_event>::unsubscribe();
	event_listener<mousedown_event>::unsubscribe();
}

bool connector_spawn_tool::is_active() const {
	return event_listener<pre_render_pass_event>::is_subscribed();
}

int connector_spawn_tool::handle(pre_render_pass_event &event) {
	if (particle_a) {
		move_mesh_to_particle(selected_a_mesh.get(), particle_a);
		hide_particle(particle_mesh, particle_a_index);
	}

	return 0;
}

int connector_spawn_tool::handle(mousedown_event &event) {
	bool is_button_relevant = event.button == GLFW_MOUSE_BUTTON_LEFT ||
		event.button == GLFW_MOUSE_BUTTON_RIGHT;

	if (event.button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (particle_a) {
			mesh_world.remove_mesh(selected_a_mesh.get());
		}

		particle_a = nullptr;
		particle_a_index = -1;
		particle_b = nullptr;
		particle_b_index = -1;

		return 0;
	} else if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (! selected_particle) {
			return 0;
		}

		if (! particle_a) {
			particle_a_index = selected_particle_index;
			particle_a = selected_particle;

			move_mesh_to_particle(selected_a_mesh.get(), particle_a);
			mesh_world.add_mesh(selected_a_mesh.get());
		} else if (particle_b_index == -1) {
			if (particle_a_index == selected_particle_index) {
				return 0;
			}

			particle_b_index = selected_particle_index;
			particle_b = selected_particle;

			connector_spawn_event rod_event(particle_a_index, particle_b_index);
			custom_bus.fire(rod_event);

			particle_a_index = -1;
			particle_b_index = -1;
			particle_a = nullptr;
			particle_b = nullptr;

			mesh_world.remove_mesh(selected_a_mesh.get());
		}
	}

	return 0;
}

int connector_spawn_tool::handle(particle_select_event &event) {
	selected_particle = &event.p;
	selected_particle_index = event.particle_index;
	particle_mesh = &event.particle_mesh;

	return 0;
}

int connector_spawn_tool::handle(particle_deselect_event &event) {
	selected_particle = nullptr;
	selected_particle_index = -1;

	return 0;
}
