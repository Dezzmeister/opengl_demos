#pragma once
#include "events.h"
#include "shader_program.h"

class material {
public:
	virtual const std::string& shader_name() const = 0;

	virtual void prepare_draw(draw_event &event, const shader_program &shader) const = 0;
};

