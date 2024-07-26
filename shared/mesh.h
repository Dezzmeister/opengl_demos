#pragma once
#include <optional>
#include "material.h"
#include "geometry.h"
#include "unique_handle.h"

class world;

class mesh {
public:

	mesh(geometry * _geom, material * _mat);

	void prepare_draw(draw_event &event, const shader_program &shader, bool include_normal = true) const;

	void set_model(const glm::mat4 &_model);

	const glm::mat4& get_model() const;

	friend bool operator<(const mesh &a, const mesh &b);
	friend bool operator==(const mesh &a, const mesh &b);

	friend class world;

private: 
	glm::mat4 model;
	glm::mat4 inv_model;
	// TODO: Make these fields const?
	geometry * geom;
	material * mat;
};

