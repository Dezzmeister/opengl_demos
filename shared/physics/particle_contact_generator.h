#pragma once
#include <vector>
#include "contacts.h"

namespace phys {
	class particle_contact_generator {
	public:
		using contact_container = std::vector<particle_contact>;

		virtual ~particle_contact_generator() = default;

		virtual void add_contacts(contact_container &contacts, real duration) const = 0;
	};
}
