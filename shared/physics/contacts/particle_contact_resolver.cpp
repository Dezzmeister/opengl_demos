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
		real max = contacts[0].calculate_separating_vel();
		uint64_t max_idx = 0;

		for (uint64_t i = 1; i < contacts.size(); i++) {
			real vs = contacts[i].calculate_separating_vel();

			// By "max" we mean the contact with the "most negative" separating velocity,
			// or the max closing velocity
			if (vs < max) {
				max = vs;
				max_idx = i;
			}
		}

		contacts[max_idx].resolve(duration);
		iterations++;
	}
}