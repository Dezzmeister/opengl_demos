#include "../contacts.h"

using namespace phys::literals;

void phys::particle_contact::resolve(real duration) {
	resolve_vel(duration);
	resolve_interpenetration();
}

phys::real phys::particle_contact::calculate_separating_vel() const {
	if (! b) {
		return phys::dot(a->vel, contact_norm);
	}

	return phys::dot(a->vel - b->vel, contact_norm);
}

void phys::particle_contact::resolve_vel(real duration) {
	if (! a->has_finite_mass() && (! b || ! b->has_finite_mass())) {
		return;
	}

	real vs = calculate_separating_vel();

	if (vs > 0) {
		return;
	}

	real vsp = -vs * restitution;
	real buildup = single_frame_buildup(duration);

	// Prevent resting objects from jittering by removing velocity caused by acceleration
	// due to forces in the current frame
	if (buildup < 0) {
		vsp += restitution * buildup;

		if (vsp < 0) {
			vsp = 0;
		}
	}

	real dvs = vsp - vs;

	if (! b || ! b->has_finite_mass()) {
		a->vel += dvs * contact_norm;
		return;
	}

	if (! a->has_finite_mass()) {
		b->vel -= dvs * contact_norm;
		return;
	}

	real ma = a->get_mass();
	real mb = b->get_mass();

	a->vel += (mb / (ma + mb)) * dvs * contact_norm;
	b->vel -= (ma / (ma + mb)) * dvs * contact_norm;
}

void phys::particle_contact::resolve_interpenetration() {
	if (! b || ! b->has_finite_mass()) {
		a->vel += penetration * contact_norm;
		return;
	}

	if (! a->has_finite_mass()) {
		b->vel -= penetration * contact_norm;
		return;
	}

	real ma = a->get_mass();
	real mb = b->get_mass();

	a->vel += (mb / (ma + mb)) * penetration * contact_norm;
	b->vel -= (ma / (ma + mb)) * penetration * contact_norm;
}

phys::real phys::particle_contact::single_frame_buildup(real duration) {
	real vel_due_to_acc = phys::dot(a->acc, contact_norm) * duration;

	if (b) {
		vel_due_to_acc -= phys::dot(b->acc, contact_norm) * duration;
	}

	return vel_due_to_acc;
}
