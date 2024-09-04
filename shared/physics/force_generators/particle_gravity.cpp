#include "../particle_force_generators.h"

phys::particle_gravity::particle_gravity(const phys::vec3 &_gravity) :
	phys::particle_force_generator(),
	gravity(_gravity)
{}

void phys::particle_gravity::update_force(phys::particle &p, phys::real duration) {
	if (! p.has_finite_mass()) {
		return;
	}

	p.force += gravity * p.get_mass();
}