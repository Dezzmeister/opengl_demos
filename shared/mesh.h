#pragma once
#include <optional>
#include "material.h"
#include "geometry.h"
#include "unique_handle.h"

class world;

// A mesh is a non-owning "view" of a geometry. A mesh's material provides the method
// of rendering the geometry, and the model matrix indicates where the geometry
// should be placed in the world.
class mesh {
public:
	mesh(const geometry * _geom, const material * _mat, int _first = 0, unsigned int _count = -1);

	void prepare_draw(draw_event &event, const shader_program &shader, bool include_normal = true) const;
	void draw() const;

	void set_model(const glm::mat4 &_model);
	void set_alpha(float _alpha);

	const glm::mat4& get_model() const;
	const material * get_material() const;
	bool has_transparency() const;

	friend bool operator<(const mesh &a, const mesh &b);
	friend bool operator==(const mesh &a, const mesh &b);

	friend class world;

private: 
	glm::mat4 model;
	glm::mat4 inv_model;
	const geometry * const geom;
	const material * const mat;
	const int first;
	const unsigned int count;
	float alpha;
};

