#pragma once
#include <vector>
#include "geometry.h"
#include "material.h"
#include "unique_handle.h"

class world;

#pragma pack(push, 1)
struct model_pair {
	glm::mat4 model;
	glm::mat4 inv_model;

	model_pair(const glm::mat4 &_model, const glm::mat4 &_inv_model);
};
#pragma pack(pop)
static_assert(sizeof(model_pair) == 16 * sizeof(float) * 2);

class instanced_mesh {
public:

	instanced_mesh(geometry * _geom, material * _mtl, size_t _instances);

	void draw(draw_event &event, const shader_program &shader) const;

	void set_model(size_t index, const glm::mat4 &_model);

	const glm::mat4& get_model(size_t index) const;

	friend bool operator==(const instanced_mesh &a, const instanced_mesh &b);

	friend class world;

private:
	geometry * geom;
	material * mtl;
	std::vector<model_pair> models;
	unique_handle<unsigned int> vbo;
	mutable bool models_need_updating;
};
