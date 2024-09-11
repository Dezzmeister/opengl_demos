#pragma once
#include <vector>
#include "particle.h"

namespace phys {
	using namespace literals;

	// A contact between two particles, or between a particle and the environment.
	class particle_contact {
	public:
		// The first particle, must not be null
		particle * a{ nullptr };
		// The optional second particle. If this is null, the contact will be treated as one
		// between a particle and the environment.
		particle * b{ nullptr };
		// The difference in position between `a` and `b`, or between `a` and the point
		// of contact (if `b` is null)
		vec3 contact_norm{ 0.0_r };
		// Coefficient of restitution
		real restitution{ 0.0_r };
		// How far `a` and `b` overlap in the direction of the contact normal
		real penetration{ 0.0_r };
		// The displacement applied to `a` during interpenetration resolution
		vec3 a_penetration_resolution{ 0.0_r };
		// The displacement applied to `b` during interpenetration resolution, if `b` is not null
		vec3 b_penetration_resolution{ 0.0_r };
		int num_resolutions{ 0 };

		// Resolves the contact and the interpenetration by applying an impulse
		// (a change in velocity) to one or both particles in the direction of the contact
		// normal
		void resolve(real duration);

		// Returns the separating velocity of both particles, i.e., the velocity with which
		// both particles are moving away from each other in the direction of the contact
		// normal. Since the contact normal is negative in the other particle's reference frame,
		// this velocity is the same in both frames.
		// If there is only one particle, this is the velocity of the first particle in the
		// direction of the contact normal.
		real calculate_separating_vel() const;

	private:
		void resolve_vel(real duration);
		void resolve_interpenetration();
		real single_frame_buildup(real duration);
	};

	class particle_contact_resolver {
	public:
		// The maximum number of contacts that can be resolved in one pass. Note that contacts may need to
		// be resolved more than once
		uint64_t max_iterations;
		// The maximum number of times that a single contact can be resolved in one pass
		uint64_t max_per_contact_iterations;

		particle_contact_resolver(
			uint64_t _max_iterations,
			uint64_t _max_per_contact_iterations
		);

		void resolve_contacts(std::vector<particle_contact> &contacts, real duration);

	private:
		uint64_t iterations{ 0 };
	};
}
