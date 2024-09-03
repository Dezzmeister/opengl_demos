#include <glad/glad.h>
#include "geometry.h"

geometry::geometry(std::vector<float> _vbo_data) :
	num_vertices(_vbo_data.size() / 8),
	vbo_data(_vbo_data),
	vao(0, [](unsigned int handle) {
		glDeleteVertexArrays(1, &handle);
	}),
	vbo(0, [](unsigned int handle) {
		glDeleteBuffers(1, &handle);
	})
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vbo_data.size() * sizeof(float), vbo_data.data(), GL_STATIC_DRAW);

	// Vertices are always (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normals are always (location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// UVs are always (location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void geometry::prepare_draw() const {
	glBindVertexArray(vao);
}

void geometry::draw(int first, unsigned int count) const {
	glDrawArrays(GL_TRIANGLES, 0, count == -1 ? (GLsizei)num_vertices : count);
}
