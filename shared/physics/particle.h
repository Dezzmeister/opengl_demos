#pragma once
#include "math.h"

namespace phys {
	class particle {
	public:
		vec3 pos;
		vec3 vel;
		vec3 acc;
		vec3 force;

		real damping;

		particle();

		void integrate(real dt);
		void set_mass(real mass);
		real get_mass() const;
		bool has_finite_mass() const;

	private:
		real inv_mass;
	};
}
