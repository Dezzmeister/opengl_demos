#pragma once
#include "events.h"
#include "unique_handle.h"
#include <memory>

class geometry {
public:
	geometry(std::vector<float> _vertices, std::vector<float> _normals);

	// Create and bind a VAO before calling this
	void create_gl();

	void draw() const;

private:
	std::vector<float> vertices;
	size_t num_vertices;
	std::vector<float> normals;
	unique_handle<unsigned int> vbo;
	unique_handle<unsigned int> normals_vbo;
};

