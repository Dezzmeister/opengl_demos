#pragma once
#include <array>
#include <bitset>
#include "../shared/physics/math.h"
#include "../shared/physics/particle.h"
#include "../shared/physics/particle_contact_generator.h"

template <const size_t N>
class spherical_particle_contact_generator : public phys::particle_contact_generator {
public:
	std::array<phys::particle, N> &particles;
	std::bitset<N> &active;

	spherical_particle_contact_generator(
		std::array<phys::particle, N> &_particles,
		std::bitset<N> &_active
	);

	void add_contacts(contact_container &contacts, phys::real duration) const override;
};

template <const size_t N>
spherical_particle_contact_generator<N>::spherical_particle_contact_generator(
	std::array<phys::particle, N> &_particles,
	std::bitset<N> &_active
) :
	particle_contact_generator(),
	particles(_particles),
	active(_active)
{}

template <const size_t N>
void spherical_particle_contact_generator<N>::add_contacts(
	contact_container &contacts,
	phys::real duration
) const {
	std::vector<size_t> active_particles{};

	for (size_t i = 0; i < N; i++) {
		if (active[i]) {
			active_particles.push_back(i);
		}
	}

	// TODO: Improve this algorithm. We can do better than O(n^2)
	for (size_t i = 0; i < active_particles.size(); i++) {
		for (size_t j = i; j < active_particles.size(); j++) {
			if (i == j) {
				continue;
			}

			phys::particle &a = particles[active_particles[i]];
			phys::particle &b = particles[active_particles[j]];
			phys::vec3 projected_a_pos = a.pos + a.vel * duration;
			phys::vec3 projected_b_pos = b.pos + b.vel * duration;
			phys::vec3 projected_dx = projected_a_pos - projected_b_pos;
			phys::real projected_dsqr = phys::dot(projected_dx, projected_dx);
			phys::real min_dsqr = (a.radius + b.radius) * (a.radius + b.radius);

			if (projected_dsqr > min_dsqr) {
				continue;
			}

			phys::particle_contact contact{
				.a = &particles[active_particles[i]],
				.b = &particles[active_particles[j]],
				.contact_norm = projected_dx,
				// TODO: Make this variable
				.restitution = 0.9_r,
				.penetration = a.radius + b.radius - std::sqrt(phys::dot(a.pos - b.pos, a.pos - b.pos))
			};

			contacts.push_back(contact);
		}
	}
}
