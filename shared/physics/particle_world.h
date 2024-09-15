#pragma once
#include <vector>
#include "contacts.h"
#include "particle.h"
#include "particle_contact_generator.h"
#include "particle_force_registry.h"
#include "particle_links.h"

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

		void add_link(particle_link * link);
		void remove_link(particle_link * link);

	private:
		std::vector<particle *> particles{};
		std::vector<particle_contact_generator *> contact_generators{};
		std::vector<particle_contact> contacts{};
		std::vector<particle_link *> links{};

		void integrate(real duration);
		void update_constraints(real duration);
	};
}
