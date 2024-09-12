#pragma once
#include "../shared/texture.h"

enum class tool_type {
	pointer,
	particle_spawn
};

struct tool {
	const texture &icon;
	tool_type type;

	tool(const texture &_icon, tool_type _type) :
		icon(_icon),
		type(_type)
	{}
};
