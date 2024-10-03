#pragma once
#include <memory>
#include <vector>
#include "constraint.h"
#include "particle.h"
#include "particle_force_registry.h"

namespace phys {
	class particle_world {
	public:
		particle_force_registry force_registry{};
		size_t solver_iterations;

		particle_world(uint64_t _solver_iterations);

		void prepare_frame();
		void run_physics(real duration);

		void add_particle(particle * p);
		void remove_particle(particle * p);

		void add_fixed_constraint(constraint * c);
		void remove_fixed_constraint(constraint * c);

		void add_constraint_generator(constraint_generator * generator);
		void remove_constraint_generator(constraint_generator * generator);

	private:
		std::vector<particle *> particles{};
		std::vector<constraint_generator *> constraint_generators{};
		std::vector<constraint *> fixed_constraints{};
		std::vector<std::unique_ptr<constraint>> collision_constraints{};
		real inv_solver_iterations;

		void damp_velocities(real duration);
		void generate_collision_constraints(real duration);
		void project_constraints();
	};
}
