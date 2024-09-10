#pragma once
#include <glm/glm.hpp>
#include "../shared/event.h"
#include "../shared/physics/particle.h"

struct sphere_spawn_event {
	const glm::vec3 pos;

	sphere_spawn_event(const glm::vec3 &_pos) : pos(_pos) {}
};

struct particle_select_event {
	const phys::particle &p;
	size_t particle_index;

	particle_select_event(const phys::particle &_p, size_t _particle_index) :
		p(_p),
		particle_index(_particle_index)
	{}
};

struct particle_deselect_event {};

using custom_event_bus = event_bus<
	sphere_spawn_event,
	particle_select_event,
	particle_deselect_event
>;
