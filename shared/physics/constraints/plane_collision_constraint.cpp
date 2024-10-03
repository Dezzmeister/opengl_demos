#include "../constraints.h"

using namespace phys::literals;

phys::plane_collision_constraint::plane_collision_constraint(
	particle * _a,
	vec3 _normal,
	vec3 _origin,
	real _restitution,
	real _friction
) :
	particle_constraint<1>(1.0_r, constraint_type::Inequality, { _a }),
	normal(_normal),
	origin(_origin),
	restitution(_restitution),
	friction(_friction),
	old_vel(_a->vel)
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
	decomposed_vec3 parts = decompose_vec3(old_vel, normal);

	vec3 new_parallel = -parts.parallel * restitution;
	vec3 new_perp = parts.perp * friction;

	a()->vel = new_parallel + new_perp;
}
