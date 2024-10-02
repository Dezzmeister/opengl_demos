#include "particle_world.h"

using namespace phys::literals;

phys::particle_world::particle_world(uint64_t _solver_iterations) :
	solver_iterations(_solver_iterations)
{}

void phys::particle_world::prepare_frame() {
	for (particle * p : particles) {
		p->force = phys::vec3(0.0_r);
		p->acc = phys::vec3(0.0_r);
	}

	collision_constraints.clear();
}

void phys::particle_world::run_physics(real duration) {
	force_registry.update_forces(duration);

	for (particle * p : particles) {
		p->vel += (duration * p->get_inv_mass() * p->force);
	}

	damp_velocities(duration);

	for (particle * p : particles) {
		p->p = p->pos + duration * p->vel;
	}

	generate_collision_constraints(duration);
	project_constraints();

	for (particle * p : particles) {
		p->vel = (p->p - p->pos) / duration;
		p->pos = p->p;
	}

	for (std::unique_ptr<constraint> &c : collision_constraints) {
		c->update_velocities(duration);
	}
}

void phys::particle_world::damp_velocities(real duration) {
	// TODO
}

void phys::particle_world::generate_collision_constraints(real dt) {
	for (constraint_generator * cg : constraint_generators) {
		cg->generate_constraints(dt, collision_constraints);
	}
}

void phys::particle_world::project_constraints() {
	for (size_t i = 0; i < solver_iterations; i++) {
		for (constraint * c : fixed_constraints) {
			c->project();
		}

		for (std::unique_ptr<constraint> &c : collision_constraints) {
			c->project();
		}
	}
}

void phys::particle_world::add_particle(particle * p) {
	particles.push_back(p);
}

void phys::particle_world::remove_particle(particle *p) {
	std::erase(particles, p);
}

void phys::particle_world::add_constraint_generator(constraint_generator * generator) {
	constraint_generators.push_back(generator);
}

void phys::particle_world::remove_constraint_generator(constraint_generator * generator) {
	std::erase(constraint_generators, generator);
}

void phys::particle_world::add_fixed_constraint(constraint * c) {
	fixed_constraints.push_back(c);
}

void phys::particle_world::remove_fixed_constraint(constraint * c) {
	std::erase(fixed_constraints, c);
}