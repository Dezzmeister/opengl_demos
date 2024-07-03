#pragma once
#include <string>
#include <variant>
#include "mesh.h"
#include "color_material.h"
#include "phong_material.h"
#include "texture_material.h"

class cube {
public:
	mesh cube_mesh;

	cube(event_buses &buses, float size, material &mat);
};

class plane {
public:
	mesh plane_mesh;

	plane(event_buses &buses, float size, material &mat);
};
