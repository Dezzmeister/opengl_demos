#include "../constraints.h"

using namespace phys::literals;

phys::plane_collision_constraint::plane_collision_constraint(
	particle * _a,
	vec3 _normal,
	vec3 _origin,
	real _restitution
) :
	particle_constraint<1>(1.0_r, constraint_type::Inequality, { _a }),
	normal(_normal),
	origin(_origin),
	restitution(_restitution)
{}

phys::real phys::plane_collision_constraint::eval_constraint() const {
	return phys::dot(a()->p - origin, normal);
}

phys::vec3 phys::plane_collision_constraint::eval_gradient(const particle &p) const {
	if (&p != a()) {
		return vec3(0.0_r);
	}

	return normal;
}

void phys::plane_collision_constraint::update_velocities(real dt) {
	// TODO
}
