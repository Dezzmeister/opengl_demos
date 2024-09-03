#include <cmath>
#include <stdexcept>
#include "particle.h"

using namespace phys::literals;

phys::particle::particle() :
	pos(0.0_r),
	vel(0.0_r),
	acc(0.0_r),
	force(0.0_r),
	damping(0.995_r),
	inv_mass(1.0_r)
{}

void phys::particle::integrate(real dt) {
	pos += (vel * dt);
	vec3 new_acc = acc + (force * inv_mass);

	vel += (new_acc * dt);
	vel *= pow(damping, dt);
}

void phys::particle::set_mass(real mass) {
	if (mass == infinity) {
		inv_mass = 0.0_r;
	} else if (mass == 0.0_r) {
		throw std::invalid_argument("Mass must be nonzero");
	} else {
		inv_mass = 1.0_r / mass;
	}
}