#include "geometry.h"
#include <glad/glad.h>

geometry::geometry(std::vector<float> _vertices, std::vector<float> _normals) :
	vertices(_vertices),
	num_vertices(vertices.size() / 3),
	normals(_normals),
	vbo(0, [](unsigned int handle) { glDeleteBuffers(1, &handle); }),
	normals_vbo(0, [](unsigned int handle) { glDeleteBuffers(1, &handle);  })
{}

void geometry::create_gl() {
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// Vertices are always (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

	// Normals are always (location = 2)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
}

void geometry::draw() const {
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)num_vertices);
}
