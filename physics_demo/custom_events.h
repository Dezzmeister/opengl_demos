#pragma once
#include <glm/glm.hpp>
#include "../shared/event.h"
#include "../shared/instanced_mesh.h"
#include "../shared/physics/particle.h"

class tool;

struct tool_register_event {
	const tool * t;

	tool_register_event(const tool * _tool) : t(_tool) {}
};

struct tool_select_event {
	const tool * t;

	tool_select_event(const tool * _tool) : t(_tool) {}
};

struct sphere_spawn_event {
	const glm::vec3 pos;

	sphere_spawn_event(const glm::vec3 &_pos) : pos(_pos) {}
};

struct particle_select_event {
	instanced_mesh &particle_mesh;
	const phys::particle &p;
	size_t particle_index;

	particle_select_event(
		instanced_mesh &_particle_mesh,
		const phys::particle &_p,
		size_t _particle_index
	) :
		particle_mesh(_particle_mesh),
		p(_p),
		particle_index(_particle_index)
	{}
};

struct particle_deselect_event {
	instanced_mesh &particle_mesh;

	particle_deselect_event(
		instanced_mesh &_particle_mesh
	) :
		particle_mesh(_particle_mesh)
	{}
};

using custom_event_bus = event_bus<
	tool_register_event,
	tool_select_event,
	sphere_spawn_event,
	particle_select_event,
	particle_deselect_event
>;
