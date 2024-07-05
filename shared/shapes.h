#pragma once
#include <string>
#include <variant>
#include "geometry.h"

namespace shapes {
	extern std::unique_ptr<geometry> cube;
	extern std::unique_ptr<geometry> plane;

	void init();
};
