#include "../constraints.h"

using namespace phys::literals;

phys::particle_collision_constraint::particle_collision_constraint(
	phys::particle * _a,
	phys::particle * _b
) :
	constraint(1.0_r, phys::constraint_type::Inequality),
	a(_a),
	b(_b)
{}

phys::real phys::particle_collision_constraint::eval_constraint() const {
	vec3 diff = a->p - b->p;

	return std::sqrt(phys::dot(diff, diff)) - (a->radius + b->radius);
}

phys::vec3 phys::particle_collision_constraint::eval_gradient(const particle &p) const {
	vec3 n = phys::normalize(a->p - b->p);

	if (&p == a) {
		return n;
	} else if (&p == b) {
		return -n;
	}

	return vec3(0.0_r);
}

void phys::particle_collision_constraint::project() {
	vec3 grad_a = eval_gradient(*a);
	vec3 grad_b = eval_gradient(*b);
	real s = eval_constraint() / (
		(a->get_inv_mass() * phys::dot(grad_a, grad_a)) +
		(b->get_inv_mass() * phys::dot(grad_b, grad_b))
	);

	vec3 da = -s * a->get_inv_mass() * grad_a;
	vec3 db = -s * b->get_inv_mass() * grad_b;

	a->p += da;
	b->p += db;
}

void phys::particle_collision_constraint::update_velocities(real dt) {
	// TODO
}