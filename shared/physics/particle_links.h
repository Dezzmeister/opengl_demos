#pragma once
#include <vector>
#include "particle.h"
#include "contacts.h"

namespace phys {
	class particle_link {
	public:
		using contact_container = std::vector<particle_contact>;

		particle * a;
		particle * b;

		particle_link(particle * _a, particle * _b);

		virtual ~particle_link() = default;

		virtual void update_constraint(real duration) = 0;
	};

	class particle_rod : public particle_link {
	public:
		real length;

		particle_rod(particle * _a, particle * _b, real _length);

		void update_constraint(real duration) override;
	};
}
