#include "../constraints.h"

using namespace phys::literals;

phys::distance_constraint::distance_constraint(
	particle * _a,
	particle * _b,
	real _distance,
	real _stiffness
) :
	constraint(_stiffness, constraint_type::Equality),
	a(_a),
	b(_b),
	distance(_distance)
{}


phys::real phys::distance_constraint::eval_constraint() const {
	vec3 dx = a->p - b->p;

	return std::sqrt(phys::dot(dx, dx)) - distance;
}

phys::vec3 phys::distance_constraint::eval_gradient(const particle &p) const {
	vec3 n = phys::normalize(a->p - b->p);

	if (&p == a) {
		return n;
	} else if (&p == b) {
		return -n;
	}

	return vec3(0.0_r);
}

void phys::distance_constraint::project() {
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

void phys::distance_constraint::update_velocities(real dt) {
	// TODO
}
