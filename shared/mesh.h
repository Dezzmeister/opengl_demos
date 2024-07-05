#pragma once
#include <optional>
#include "material.h"
#include "geometry.h"
#include "unique_handle.h"

class mesh : public event_listener<draw_event> {
public:
	glm::mat4 model;

	mesh(event_buses &_buses, geometry &_geom, material &_mat);

	void add_to_world();

	int handle(draw_event &event) override;

private: 
	event_buses &buses;
	geometry &geom;
	material &mat;
};

