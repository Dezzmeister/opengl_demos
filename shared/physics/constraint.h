#pragma once
#include <memory>
#include <vector>
#include "math.h"
#include "particle.h"

namespace phys {
	enum class constraint_type {
		Equality,
		Inequality
	};

	class constraint {
	public:
		real stiffness;
		constraint_type type;

		constraint(real _stiffness, constraint_type _type);
		virtual ~constraint() = default;

		virtual real eval_constraint() const = 0;
		virtual vec3 eval_gradient(const particle &p) const = 0;

		// TODO: Make this default/generic
		virtual void project() = 0;
		virtual void update_velocities(real dt) = 0;
	};

	class constraint_generator {
	public:
		virtual ~constraint_generator() = default;

		virtual void generate_constraints(real dt, std::vector<std::unique_ptr<constraint>> &constraints) = 0;
	};
}