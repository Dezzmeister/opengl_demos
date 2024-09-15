#include "../particle_links.h"

phys::particle_link::particle_link(particle * _a, particle * _b) :
	a(_a),
	b(_b)
{}

phys::particle_rod::particle_rod(
	particle * _a,
	particle * _b,
	real _length
) :
	particle_link(_a, _b),
	length(_length)
{}

void phys::particle_rod::generate_contacts(contact_container &contacts, real duration) {
	vec3 proj_a_pos = a->pos + a->vel * duration;
	vec3 proj_b_pos = b->pos + b->vel * duration;
	vec3 proj_dx = proj_a_pos - proj_b_pos;
	real proj_len = std::sqrt(phys::dot(proj_dx, proj_dx));

	if (proj_len == length) {
		return;
	}

	if (proj_len < length) {
		contacts.push_back({
			.a = a,
			.b = b,
			.contact_norm = proj_dx,
			.restitution = 0,
			.penetration = length - proj_len
		});
	} else {
		contacts.push_back({
			.a = a,
			.b = b,
			.contact_norm = -proj_dx,
			.restitution = 0,
			.penetration = proj_len - length
		});
	}
}
