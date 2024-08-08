#pragma once
#include "events.h"
#include "rendering.h"
#include "shader_program.h"

class material {
public:
	virtual ~material() = default;

	virtual const std::string& shader_name() const = 0;

	virtual void prepare_draw(draw_event &event, const shader_program &shader, render_pass_state &render_pass) const = 0;
};

