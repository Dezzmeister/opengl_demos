#pragma once
#include "events.h"
#include "unique_handle.h"
#include <memory>

class geometry {
public:
	// Vertices, normals, and UVs are interleaved
	geometry(std::vector<float> _vbo_data);

	void prepare_draw() const;

	void draw() const;

private:
	std::vector<float> vbo_data;
	size_t num_vertices;
	unique_handle<unsigned int> vao;
	unique_handle<unsigned int> vbo;
};

