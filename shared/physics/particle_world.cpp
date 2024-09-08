#include "particle_world.h"

using namespace phys::literals;

phys::particle_world::particle_world(uint64_t _max_iterations) :
	contact_resolver(_max_iterations)
{}

void phys::particle_world::integrate(real duration) {
	for (particle * p : particles) {
		p->integrate(duration);
	}
}

void phys::particle_world::prepare_frame() {
	for (particle * p : particles) {
		p->force = phys::vec3(0.0_r);
		p->acc = phys::vec3(0.0_r);
	}
}

void phys::particle_world::run_physics(real duration) {
	force_registry.update_forces(duration);

	integrate(duration);

	contacts.clear();

	for (particle_contact_generator * cg : contact_generators) {
		cg->add_contacts(contacts, duration);
	}

	contact_resolver.resolve_contacts(contacts, duration);
}

void phys::particle_world::add_particle(particle * p) {
	particles.push_back(p);
}

void phys::particle_world::remove_particle(particle *p) {
	std::erase(particles, p);
}

void phys::particle_world::add_contact_generator(particle_contact_generator * generator) {
	contact_generators.push_back(generator);
}

void phys::particle_world::remove_contact_generator(particle_contact_generator * generator) {
	std::erase(contact_generators, generator);
}