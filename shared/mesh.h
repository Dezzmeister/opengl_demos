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

	mesh(const geometry * _geom, const material * _mat);

	void prepare_draw(draw_event &event, const shader_program &shader, bool include_normal = true) const;

	void set_model(const glm::mat4 &_model);

	const glm::mat4& get_model() const;

	friend bool operator<(const mesh &a, const mesh &b);
	friend bool operator==(const mesh &a, const mesh &b);

	friend class world;

private: 
	glm::mat4 model;
	glm::mat4 inv_model;
	const geometry * const geom;
	const material * const mat;
};

