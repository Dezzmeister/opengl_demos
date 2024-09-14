#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "../shared/geometry.h"
#include "../shared/phong_color_material.h"
#include "../shared/physics/math.h"

// Ruby
extern const phong_color_material sphere_mtl;

extern const phys::real sphere_radius;
extern const glm::mat4 sphere_scale;
extern std::unique_ptr<geometry> sphere_geom;

extern void init_constants();
