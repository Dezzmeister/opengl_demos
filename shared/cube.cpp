#include "shapes.h"

const std::vector<float> cube::uvs{
	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f
};

static std::vector<float> make_cube_verts(float size) {
	const float s = size / 2;

	return {
		// Front
		-s, -s, -s,
		s, s, -s,
		s, -s, -s,
		s, s, -s,
		-s, -s, -s,
		-s, s, -s,

		// Back
		-s, -s, s,
		s, -s, s,
		s, s, s,
		s, s, s,
		-s, s, s,
		-s, -s, s,

		// Right
		-s, s, s,
		-s, s, -s,
		-s, -s, -s,
		-s, -s, -s,
		-s, -s, s,
		-s, s, s,

		// Left
		s, s, s,
		s, -s, s,
		s, s, -s,
		s, s, -s,
		s, -s, s,
		s, -s, -s,

		// Top
		s, s, s,
		-s, s, -s,
		-s, s, s,
		s, s, s,
		s, s, -s,
		-s, s, -s,	

		// Bottom
		-s, -s, -s,
		s, -s, s,
		-s, -s, s,
		-s, -s, -s,
		s, -s, -s,
		s, -s, s,
	};
};

static std::vector<float> cube_normals{
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
};

cube::cube(event_buses &buses, float size, material &_mat) :
	cube_mesh(buses, geometry(make_cube_verts(size), cube_normals), _mat)
{}
