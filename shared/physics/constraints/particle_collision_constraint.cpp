#include "../constraints.h"

using namespace phys::literals;

phys::particle_collision_constraint::particle_collision_constraint(
	particle * _a,
	particle * _b,
	real _restitution,
	real _friction
) :
	particle_constraint<2>(1.0_r, phys::constraint_type::Inequality, { _a, _b }),
	restitution(_restitution),
	friction(_friction),
	a_old_vel(_a->vel),
	b_old_vel(_b->vel),
	a_old_pos(_a->p),
	b_old_pos(_b->p)
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
	// TODO: Uncomment this and fix it
	/*
	if (a_old_pos == b_old_pos) {
		return;
	}

	real ma = a()->get_mass();
	real mb = b()->get_mass();
	real m_total = ma + mb;

	if (m_total == 0.0_r) {
		return;
	}

	vec3 n = phys::normalize(b_old_pos - a_old_pos);
	decomposed_vec3 a_parts = decompose_vec3(a_old_vel, n);
	decomposed_vec3 b_parts = decompose_vec3(b_old_vel, n);
	vec3 ua = (1 - restitution) * (mb / m_total) * b_parts.parallel + ((restitution * mb + ma) / m_total) * a_parts.parallel;
	vec3 ub = (1 - restitution) * (ma / m_total) * a_parts.parallel + ((restitution * ma + mb) / m_total) * b_parts.parallel;

	real f = 1 - friction * dt;

	a()->vel = -ua + f * a_parts.perp;
	b()->vel = -ub + f * b_parts.perp;
	*/
}