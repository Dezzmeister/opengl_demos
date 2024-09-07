#pragma once
#include <vector>
#include "particle.h"

namespace phys {
	using namespace literals;

	class particle_contact {
	public:
		particle * a{ nullptr };
		particle * b{ nullptr };
		vec3 contact_norm{ 0.0_r };
		real restitution{ 0.0_r };
		real penetration{ 0.0_r };

		void resolve(real duration);
		real calculate_separating_vel() const;

	private:
		void resolve_vel(real duration);
		void resolve_interpenetration();
		real single_frame_buildup(real duration);
	};

	class particle_contact_resolver {
	public:
		uint64_t max_iterations;

		particle_contact_resolver(uint64_t _max_iterations);

		void resolve_contacts(std::vector<particle_contact> &contacts, real duration);

	private:
		uint64_t iterations{ 0 };
	};
}
