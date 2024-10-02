#pragma once
#include "constraint.h"

namespace phys {
	class distance_constraint : public constraint {
	public:
		particle * a;
		particle * b;
		real distance;

		distance_constraint(
			particle * _a,
			particle * _b,
			real _distance,
			real _stiffness
		);

		real eval_constraint() const override;
		vec3 eval_gradient(const particle &p) const override;
		void project() override;
		void update_velocities(real dt) override;
	};

	class plane_collision_constraint : public constraint {
	public:
		particle * p;
		vec3 normal;
		vec3 origin;
		real restitution;

		plane_collision_constraint(
			particle * _p,
			vec3 _normal,
			vec3 _origin,
			real _restitution
		);

		real eval_constraint() const override;
		vec3 eval_gradient(const particle &_p) const override;
		void project() override;
		void update_velocities(real dt) override;
	};

	template <typename particle_container>
	class plane_collision_constraint_generator : public constraint_generator {
	public:
		plane_collision_constraint_generator(
			particle_container &_particles,
			vec3 _normal,
			vec3 _origin,
			real _restitution
		);

		void generate_constraints(real dt, std::vector<std::unique_ptr<constraint>> &constraints) override;

	private:
		particle_container &particles;
		vec3 normal;
		vec3 origin;
		real restitution;
	};

	class particle_collision_constraint : public constraint {
	public:
		particle * a;
		particle * b;

		particle_collision_constraint(
			phys::particle * _a,
			phys::particle * _b
		);

		real eval_constraint() const override;
		vec3 eval_gradient(const particle &p) const override;
		void project() override;
		void update_velocities(real dt) override;
	};
}

template <typename particle_container>
phys::plane_collision_constraint_generator<particle_container>::plane_collision_constraint_generator(
	particle_container &_particles,
	vec3 _normal,
	vec3 _origin,
	real _restitution
) :
	particles(_particles),
	normal(_normal),
	origin(_origin),
	restitution(_restitution)
{}

template <typename particle_container>
void phys::plane_collision_constraint_generator<particle_container>::generate_constraints(
	real dt,
	std::vector<std::unique_ptr<constraint>> &constraints
) {
	using namespace phys::literals;

	for (particle &p : particles) {
		if (phys::dot(p.p - origin, normal) < 0.0_r) {
			constraints.push_back(std::make_unique<phys::plane_collision_constraint>(
				&p,
				normal,
				origin,
				restitution
			));
		}
	}
}
