#include "../constraints.h"

using namespace phys::literals;

phys::particle_collision_constraint::particle_collision_constraint(
	particle * _a,
	particle * _b,
	real _restitution,
	real _friction
) :
	particle_constraint<2>(1.0_r, phys::constraint_type::Inequality, { _a, _b }),
	a_old_vel(_a->vel),
	b_old_vel(_b->vel),
	restitution(_restitution),
	friction(_friction)
{}

phys::real phys::particle_collision_constraint::eval_constraint() const {
	vec3 diff = a()->p - b()->p;

	return std::sqrt(phys::dot(diff, diff)) - (a()->radius + b()->radius);
}

phys::vec3 phys::particle_collision_constraint::eval_gradient(const particle &p) const {
	if (a()->p == b()->p) {
		return vec3(0.0_r);
	}

	vec3 n = phys::normalize(a()->p - b()->p);

	if (&p == a()) {
		return n;
	} else if (&p == b()) {
		return -n;
	}

	return vec3(0.0_r);
}

void phys::particle_collision_constraint::update_velocities(real dt) {
	// TODO
}