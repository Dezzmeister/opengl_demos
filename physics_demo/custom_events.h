#pragma once
#include <glm/glm.hpp>
#include "../shared/event.h"

struct sphere_spawn_event {
	const glm::vec3 pos;

	sphere_spawn_event(const glm::vec3 &_pos) : pos(_pos) {}
};

using custom_event_bus = event_bus<
	sphere_spawn_event
>;
