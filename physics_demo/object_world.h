#pragma once
#include <array>
#include <bitset>
#include <memory>
#include "../shared/events.h"
#include "../shared/instanced_mesh.h"
#include "../shared/phong_color_material.h"
#include "../shared/physics/particle.h"
#include "../shared/physics/particle_contact_generators.h"
#include "../shared/physics/particle_force_generators.h"
#include "../shared/physics/particle_world.h"
#include "../shared/shapes.h"
#include "../shared/world.h"
#include "custom_events.h"
#include "spherical_particle_contact_generator.h"

using namespace phys::literals;

namespace {
	constexpr size_t MAX_PHYS_ITERATIONS = 8;

	// Ruby
	phong_color_material sphere_mtl{
		phong_color_material_properties{
			glm::vec3(0.1745, 0.01175, 0.01175),
			glm::vec3(0.61424, 0.04136, 0.04136),
			glm::vec3(0.727811, 0.626959, 0.626959),
			128 * 0.6f
		}
	};

	// Chrome
	phong_color_material rod_mtl{
		phong_color_material_properties{
			glm::vec3(0.25, 0.25, 0.25),
			glm::vec3(0.4, 0.4, 0.4),
			glm::vec3(0.774597, 0.774597, 0.774597),
			128 * 0.6f
		}
	};

	// Obsidian
	phong_color_material floor_mtl{
		phong_color_material_properties{
			glm::vec3(0.05375, 0.05, 0.06625),
			glm::vec3(0.18275, 0.17, 0.22525),
			glm::vec3(0.332741, 0.328634, 0.346435),
			128 * 0.3f
		},
	};

	constexpr float floor_y = -1.0f;
	constexpr float sphere_radius = 0.1f;
	constexpr float cyl_radius = 0.05f;
	const glm::mat4 sphere_scale = glm::scale(glm::identity<glm::mat4>(),
		glm::vec3(sphere_radius / 0.5f, sphere_radius / 0.5f, sphere_radius / 0.5f));

	template <const size_t N>
	struct world_state {
		geometry sphere_geom;
		geometry rod_geom;
		instanced_mesh sphere_meshes;
		instanced_mesh rod_meshes;

		std::array<phys::particle, N> particles{};
		std::bitset<N> active{};

		world_state(
			world &_mesh_world,
			geometry _sphere_geom,
			geometry _rod_geom,
			const glm::vec3 &_hiding_pos
		);

		void update_meshes();

		void hide_sphere(size_t i);
		void hide_rod(size_t i);

	private:
		// Any unused mesh instances will be moved to a location
		// far from anything that the player is likely to encounter
		glm::mat4 hiding_transform;
	};
}

template <const size_t N>
class object_world :
	public event_listener<pre_render_pass_event>,
	public event_listener<post_render_pass_event>,
	public event_listener<sphere_spawned_event>
{
public:
	object_world(event_buses &_buses, custom_event_bus &_custom_bus, world &_mesh_world);

	int handle(pre_render_pass_event &event) override;
	int handle(post_render_pass_event &event) override;
	int handle(sphere_spawned_event &event) override;

private:
	std::unique_ptr<world_state<N>> state;
	world &mesh_world;
	phys::particle_world phys_world;

	std::unique_ptr<directional_light> light{};
	std::unique_ptr<mesh> floor{};
	std::unique_ptr<phys::particle_plane_contact_generator<std::array<phys::particle, N>>> floor_contact_generator;
	std::unique_ptr<spherical_particle_contact_generator<N>> sphere_contact_generator;
	std::unique_ptr<phys::particle_gravity> gravity_generator;

	int64_t next_inactive_particle() const;
};

template <const size_t N>
world_state<N>::world_state(
	world &_mesh_world,
	geometry _sphere_geom,
	geometry _rod_geom,
	const glm::vec3 &_hiding_pos
) :
	sphere_geom(std::move(_sphere_geom)),
	rod_geom(std::move(_rod_geom)),
	sphere_meshes(&sphere_geom, &sphere_mtl, N),
	rod_meshes(&rod_geom, &rod_mtl, N),
	hiding_transform(glm::translate(glm::identity<glm::mat4>(), _hiding_pos))
{
	for (size_t i = 0; i < N; i++) {
		hide_sphere(i);
		hide_rod(i);
	}

	_mesh_world.add_instanced_mesh(&sphere_meshes);
	_mesh_world.add_instanced_mesh(&rod_meshes);
}

template <const size_t N>
void world_state<N>::update_meshes() {
	for (size_t i = 0; i < N; i++) {
		if (! active[i]) {
			// TODO: Rods
			hide_sphere(i);
			continue;
		}

		sphere_meshes.set_model(i, glm::translate(glm::identity<glm::mat4>(),
			phys::to_glm<glm::vec3>(particles[i].pos)
			) * sphere_scale
		);
	}
}

template <const size_t N>
void world_state<N>::hide_sphere(size_t i) {
	sphere_meshes.set_model(i, hiding_transform);
}

template <const size_t N>
void world_state<N>::hide_rod(size_t i) {
	rod_meshes.set_model(i, hiding_transform);
}

template <const size_t N>
object_world<N>::object_world(
	event_buses &_buses,
	custom_event_bus &_custom_bus,
	world &_mesh_world
) :
	event_listener<pre_render_pass_event>(&_buses.render, -10),
	event_listener<post_render_pass_event>(&_buses.render),
	event_listener<sphere_spawned_event>(&_custom_bus),
	state(std::make_unique<world_state<N>>(
		_mesh_world,
		shapes::make_sphere(20, 10, true),
		shapes::make_cylinder(20, true),
		glm::vec3(0.0f, -10000.0f, 0.0f)
	)),
	mesh_world(_mesh_world),
	phys_world(MAX_PHYS_ITERATIONS),
	floor_contact_generator(std::make_unique<phys::particle_plane_contact_generator<std::array<phys::particle, N>>>(
		state->particles,
		phys::vec3(0.0_r, 1.0_r, 0.0_r),
		phys::vec3(0.0_r, floor_y + sphere_radius, 0.0_r),
		0.9_r
	)),
	sphere_contact_generator(std::make_unique<spherical_particle_contact_generator<N>>(
		state->particles,
		state->active
	)),
	gravity_generator(std::make_unique<phys::particle_gravity>(
		phys::vec3(0.0_r, -9.8_r, 0.0_r)
	))
{
	event_listener<pre_render_pass_event>::subscribe();
	event_listener<post_render_pass_event>::subscribe();
	event_listener<sphere_spawned_event>::subscribe();

	light = std::make_unique<directional_light>(
		glm::normalize(glm::vec3(1, -3, 1)),
		light_properties(
			glm::vec3(1.0f),
			glm::vec3(1.0f),
			glm::vec3(1.0f)
		),
		directional_shadow_caster_properties(
			glm::normalize(glm::vec3(1, -3, 1)),
			10.0f,
			100.0f,
			1024
		)
	);

	light->set_casts_shadow(true);

	mesh_world.add_light(light.get());

	floor = std::make_unique<mesh>(shapes::plane.get(), &floor_mtl);
	floor->set_model(glm::translate(glm::identity<glm::mat4>(), glm::vec3(
		0.0f,
		floor_y,
		0.0f
	)) * glm::scale(glm::identity<glm::mat4>(), glm::vec3(100.0f, 1.0f, 100.0f)));

	mesh_world.add_mesh(floor.get());

	phys_world.add_contact_generator(floor_contact_generator.get());
	phys_world.add_contact_generator(sphere_contact_generator.get());
}

template <const size_t N>
int object_world<N>::handle(pre_render_pass_event &event) {
	long long millis = event.delta.count();

	phys_world.prepare_frame();
	phys_world.run_physics(((phys::real)millis) / 1000.0_r);
	state->update_meshes();

	return 0;
}

template <const size_t N>
int object_world<N>::handle(post_render_pass_event &event) {
	return 0;
}

template <const size_t N>
int object_world<N>::handle(sphere_spawned_event &event) {
	int64_t i = next_inactive_particle();

	if (i == -1) {
		std::cout << "Max particles reached (" << N << ")" << std::endl;
		// TODO: Do something in the game world
		return 1;
	}

	state->particles[i].pos = event.pos;
	state->particles[i].radius = sphere_radius;
	state->active[i] = true;
	phys_world.add_particle(&state->particles[i]);
	phys_world.force_registry.add(&state->particles[i], gravity_generator.get());

	return 0;
}

template <const size_t N>
int64_t object_world<N>::next_inactive_particle() const {
	for (size_t i = 0; i < N; i++) {
		if (! state->active[i]) {
			return i;
		}
	}

	return -1;
}
