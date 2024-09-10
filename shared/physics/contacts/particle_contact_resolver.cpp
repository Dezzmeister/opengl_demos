#include "../contacts.h"

phys::particle_contact_resolver::particle_contact_resolver(uint64_t _max_iterations) :
	max_iterations(_max_iterations)
{}

void phys::particle_contact_resolver::resolve_contacts(std::vector<particle_contact> &contacts, real duration) {
	iterations = 0;

	if (! contacts.size()) {
		return;
	}

	while (iterations < max_iterations) {
		real max = infinity;
		uint64_t max_idx = contacts.size();

		for (uint64_t i = 0; i < contacts.size(); i++) {
			real vs = contacts[i].calculate_separating_vel();

			// By "max" we mean the contact with the "most negative" separating velocity,
			// or the max closing velocity
			if (vs < max && (vs < 0 || contacts[i].penetration > 0)) {
				max = vs;
				max_idx = i;
			}
		}

		if (max_idx == contacts.size()) {
			break;
		}

		contacts[max_idx].resolve(duration);

		for (size_t i = 0; i < contacts.size(); i++) {
			particle_contact &contact = contacts[i];

			if (contact.a == contacts[max_idx].a) {
				contact.penetration -= phys::dot(contacts[max_idx].a_penetration_resolution, contact.contact_norm);
			} else if (contact.a == contacts[max_idx].b) {
				contact.penetration -= phys::dot(contacts[max_idx].b_penetration_resolution, contact.contact_norm);
			}

			if (contact.b) {
				if (contact.b == contacts[max_idx].a) {
					contact.penetration += phys::dot(contacts[max_idx].a_penetration_resolution, contact.contact_norm);
				} else if (contact.b == contacts[max_idx].b) {
					contact.penetration += phys::dot(contacts[max_idx].b_penetration_resolution, contact.contact_norm);
				}
			}
		}

		iterations++;
	}
}