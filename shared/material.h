#pragma once
#include "events.h"
#include "shader_program.h"

class material {
public:
	virtual void create_gl() = 0;

	virtual void draw(draw_event &event, const shader_program &shader) = 0;

	virtual const std::string& shader_name() const = 0;
};

