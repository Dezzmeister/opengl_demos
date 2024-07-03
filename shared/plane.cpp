#include "shapes.h"

const std::vector<float> plane::uvs{
	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

static std::vector<float> make_vertices(float size) {
	const float s = size / 2;

	return {
		-s, 0, -s,
		s, 0, s,
		s, 0, -s,

		-s, 0, -s,
		-s, 0, s,
		s, 0, s
	};
}

static std::vector<float> plane_normals{
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0
};

plane::plane(event_buses &buses, float size, material &mat) :
	plane_mesh(buses, geometry(make_vertices(size), plane_normals), mat)
{}
