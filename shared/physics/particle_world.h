#pragma once
#include <vector>
#include "contacts.h"
#include "particle.h"
#include "particle_contact_generator.h"
#include "particle_force_registry.h"

namespace phys {
	class particle_world {
	public:
		particle_force_registry force_registry{};
		particle_contact_resolver contact_resolver;

		particle_world(uint64_t _max_iterations);

		void prepare_frame();
		void run_physics(real duration);

		void add_particle(particle * p);
		void remove_particle(particle * p);

		void add_contact_generator(particle_contact_generator * generator);
		void remove_contact_generator(particle_contact_generator * generator);

	private:
		std::vector<particle *> particles{};
		std::vector<particle_contact_generator *> contact_generators{};
		std::vector<particle_contact> contacts{};

		void integrate(real duration);
	};
}
