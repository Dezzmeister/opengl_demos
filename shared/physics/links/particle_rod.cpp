#include "../particle_links.h"

phys::particle_link::particle_link(particle * _a, particle * _b) :
	a(_a),
	b(_b)
{}

phys::particle_rod::particle_rod(
	particle * _a,
	particle * _b,
	real _length
) :
	particle_link(_a, _b),
	length(_length)
{}

void phys::particle_rod::update_constraint(real duration) {
	vec3 r = a->pos - b->pos;
	real d = std::sqrt(phys::dot(r, r));
	real disp = length - d;

	if (disp == 0.0_r) {
		return;
	}

	vec3 dir = phys::normalize(r);
	vec3 v = a->vel - b->vel;
	real inv_m = a->get_inv_mass() + b->get_inv_mass();

	if (inv_m == 0.0_r) {
		return;
	}

	float v_in_dir = phys::dot(v, dir);
	float bias_f = 0.01f;
	float bias = -(bias_f / duration) * disp;
	float lambda = -(v_in_dir + bias) / inv_m;

	vec3 a_impulse = dir * lambda;
	vec3 b_impulse = -dir * lambda;

	a->vel += a_impulse * a->get_inv_mass();
	b->vel += b_impulse * b->get_inv_mass();
}
