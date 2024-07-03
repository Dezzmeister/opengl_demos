#pragma once
#include <string>
#include <variant>
#include "mesh.h"
#include "material.h"

class cube {
public:
	const static std::vector<float> uvs;

	mesh cube_mesh;

	cube(event_buses &buses, float size, material &mat);
};

class plane {
public:
	const static std::vector<float> uvs;

	mesh plane_mesh;

	plane(event_buses &buses, float size, material &mat);
};
