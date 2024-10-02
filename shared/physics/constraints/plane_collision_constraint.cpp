#include "../constraints.h"

using namespace phys::literals;

phys::plane_collision_constraint::plane_collision_constraint(
	particle * _p,
	vec3 _normal,
	vec3 _origin,
	real _restitution
) :
	constraint(1.0_r, constraint_type::Inequality),
	p(_p),
	normal(_normal),
	origin(_origin),
	restitution(_restitution)
{}

phys::real phys::plane_collision_constraint::eval_constraint() const {
	return phys::dot(p->p - origin, normal);
}

phys::vec3 phys::plane_collision_constraint::eval_gradient(const particle &_p) const {
	if (p != &_p) {
		return vec3(0.0_r);
	}

	return normal;
}

void phys::plane_collision_constraint::project() {
	// TODO: Simplify this
	vec3 grad = eval_gradient(*p);
	real s = eval_constraint() / (p->get_inv_mass() * phys::dot(grad, grad));
	vec3 dp = -s * p->get_inv_mass() * grad;

	p->p += dp;
}

void phys::plane_collision_constraint::update_velocities(real dt) {
	// TODO
}
