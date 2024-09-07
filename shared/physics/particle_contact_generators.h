#pragma once
#include <iterator>
#include "particle_contact_generator.h"

namespace phys {
	template <typename particle_container>
	class particle_plane_contact_generator : public particle_contact_generator {
	public:
		particle_container &particles;
		vec3 normal;
		vec3 origin;
		real restitution;

		particle_plane_contact_generator(
			particle_container &_particles,
			vec3 _normal,
			vec3 _origin,
			real _restitution
		);

		void add_contacts(contact_container &contacts) const override;
	};
}

template <typename particle_container>
phys::particle_plane_contact_generator<particle_container>::particle_plane_contact_generator(
	particle_container &_particles,
	vec3 _normal,
	vec3 _origin,
	real _restitution
) :
	particle_contact_generator(),
	particles(_particles),
	normal(_normal),
	origin(_origin),
	restitution(_restitution)
{}

template <typename particle_container>
void phys::particle_plane_contact_generator<particle_container>::add_contacts(contact_container &contacts) const {
	for (particle &p : particles) {
		real dist = phys::dot(p.pos - origin, normal);

		if (dist < 0) {
			particle_contact contact{
				.a = &p,
				.b = nullptr,
				.contact_norm = normal,
				.restitution = restitution,
				.penetration = -dist
			};

			contacts.push_back(contact);
		}
	}
}
