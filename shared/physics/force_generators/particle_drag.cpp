#include "../particle_force_generators.h"
#include "math.h"

phys::particle_drag::particle_drag(phys::real _k1, phys::real _k2) :
	phys::particle_force_generator(),
	k1(_k1),
	k2(_k2)
{}

void phys::particle_drag::update_force(phys::particle &p, phys::real duration) {
	vec3 norm_vel = phys::normalize(p.vel);
	real speed_sqr = phys::dot(p.vel, p.vel);
	real speed = std::sqrt(speed_sqr);

	vec3 f = -norm_vel * (k1 * speed + k2 * speed_sqr);

	p.force += f;
}