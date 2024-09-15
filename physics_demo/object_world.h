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
#include "constants.h"
#include "custom_events.h"
#include "raycast.h"
#include "spherical_particle_contact_generator.h"

using namespace phys::literals;

namespace {
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
	constexpr float cyl_radius = 0.05f;

	template <const size_t N>
	struct world_state {
		geometry rod_geom;
		instanced_mesh sphere_meshes;
		instanced_mesh rod_meshes;

		std::array<phys::particle, N> particles{};
		std::bitset<N> active{};

		world_state(
			world &_mesh_world,
			geometry _rod_geom,
			const glm::vec3 &_hiding_pos
		);

		void update_meshes();

		void hide_sphere(size_t i);
		void hide_rod(size_t i);

		// Returns true if the selection was successful (i.e. it changed
		// state), false if not.
		bool select_particle(int64_t i);

	private:
		// Any unused instanced mesh instances will be moved to a location
		// far from anything that the player is likely to encounter
		glm::mat4 hiding_transform;
		int64_t selected_particle{ -1 };
		world &mesh_world;

		glm::mat4 particle_transform_mat(size_t i) const;
	};

	// A raycast hit result
	struct hit_result {
		size_t i{ 0 };
		phys::real t{ -1.0_r };
	};
}

template <const size_t N>
class object_world :
	public event_listener<pre_render_pass_event>,
	public event_listener<post_render_pass_event>,
	public event_listener<particle_spawn_event>,
	public event_listener<keydown_event>,
	public event_listener<player_spawn_event>,
	public event_listener<player_move_event>,
	public event_listener<player_look_event>
{
public:
	object_world(
		event_buses &_buses,
		custom_event_bus &_custom_bus,
		world &_mesh_world,
		short _pause_key,
		short _step_key
	);

	int handle(pre_render_pass_event &event) override;
	int handle(post_render_pass_event &event) override;
	int handle(particle_spawn_event &event) override;
	int handle(keydown_event &event) override;
	int handle(player_spawn_event &event) override;
	int handle(player_move_event &event) override;
	int handle(player_look_event &event) override;

private:
	std::unique_ptr<world_state<N>> state;
	world &mesh_world;
	phys::particle_world phys_world;
	custom_event_bus &custom_bus;

	std::unique_ptr<directional_light> light{};
	std::unique_ptr<mesh> floor{};

	std::unique_ptr<phys::particle_plane_contact_generator<std::array<phys::particle, N>>> floor_contact_generator;
	std::unique_ptr<spherical_particle_contact_generator<N>> sphere_contact_generator;
	std::unique_ptr<phys::particle_gravity> gravity_generator;

	glm::vec3 player_pos{};
	glm::vec3 player_dir{};
	std::vector<hit_result> hit_results{};

	short pause_key;
	short step_key;
	bool paused{ false };
	bool step{ false };

	int64_t next_inactive_particle() const;
	void do_raycast_and_update();
};

template <const size_t N>
world_state<N>::world_state(
	world &_mesh_world,
	geometry _rod_geom,
	const glm::vec3 &_hiding_pos
) :
	rod_geom(std::move(_rod_geom)),
	sphere_meshes(sphere_geom.get(), &sphere_mtl, N),
	rod_meshes(&rod_geom, &rod_mtl, N),
	hiding_transform(glm::translate(glm::identity<glm::mat4>(), _hiding_pos)),
	mesh_world(_mesh_world)
{
	for (size_t i = 0; i < N; i++) {
		hide_sphere(i);
		hide_rod(i);
	}

	mesh_world.add_instanced_mesh(&sphere_meshes);
	mesh_world.add_instanced_mesh(&rod_meshes);
}

template <const size_t N>
void world_state<N>::update_meshes() {
	for (size_t i = 0; i < N; i++) {
		if (! active[i]) {
			// TODO: Rods
			hide_sphere(i);
			continue;
		}

		sphere_meshes.set_model(i, particle_transform_mat(i));
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
bool world_state<N>::select_particle(int64_t i) {
	if (selected_particle == i) {
		return false;
	}

	selected_particle = i;

	return true;
}

template <const size_t N>
glm::mat4 world_state<N>::particle_transform_mat(size_t i) const {
	return glm::translate(
		glm::identity<glm::mat4>(),
		phys::to_glm<glm::vec3>(particles[i].pos)
	) * sphere_scale;
}

template <const size_t N>
object_world<N>::object_world(
	event_buses &_buses,
	custom_event_bus &_custom_bus,
	world &_mesh_world,
	short _pause_key,
	short _step_key
) :
	event_listener<pre_render_pass_event>(&_buses.render, -10),
	event_listener<post_render_pass_event>(&_buses.render),
	event_listener<particle_spawn_event>(&_custom_bus),
	event_listener<keydown_event>(&_buses.input),
	event_listener<player_spawn_event>(&_buses.player),
	event_listener<player_move_event>(&_buses.player),
	event_listener<player_look_event>(&_buses.player),
	state(std::make_unique<world_state<N>>(
		_mesh_world,
		shapes::make_cylinder(20, true),
		glm::vec3(0.0f, -10000.0f, 0.0f)
	)),
	mesh_world(_mesh_world),
	// Setting max_iterations = N here helps prevent particles in resting contact
	// from jittering. Resting contact jittering can happen when the number of
	// contacts is greater than max_iterations
	phys_world(N),
	custom_bus(_custom_bus),
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
	)),
	pause_key(_pause_key),
	step_key(_step_key)
{
	event_listener<pre_render_pass_event>::subscribe();
	event_listener<post_render_pass_event>::subscribe();
	event_listener<particle_spawn_event>::subscribe();
	event_listener<keydown_event>::subscribe();
	event_listener<player_spawn_event>::subscribe();
	event_listener<player_move_event>::subscribe();
	event_listener<player_look_event>::subscribe();

	light = std::make_unique<directional_light>(
		glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)),
		light_properties(
			glm::vec3(1.0f),
			glm::vec3(1.0f),
			glm::vec3(1.0f)
		),
		directional_shadow_caster_properties(
			glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)),
			10.0f,
			100.0f,
			2048
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
	if (! paused || (paused && step)) {
		long long millis = event.delta.count();
		phys::real seconds = ((phys::real)millis) / 1000.0_r;

		phys_world.prepare_frame();
		// Clamp the timestep so that physics doesn't go crazy when the window is moved
		// or resized, or when a frame takes too long
		phys_world.run_physics(std::min(seconds, 0.02_r));
		step = false;
	}

	do_raycast_and_update();
	state->update_meshes();

	return 0;
}

template <const size_t N>
int object_world<N>::handle(post_render_pass_event &event) {

	return 0;
}

template <const size_t N>
int object_world<N>::handle(particle_spawn_event &event) {
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
int object_world<N>::handle(keydown_event &event) {
	if (! event.is_mouse_locked) {
		return 0;
	}

	if (event.key == pause_key) {
		paused = ! paused;
	} else if (event.key == step_key) {
		step = true;
	}

	return 0;
}

template <const size_t N>
int object_world<N>::handle(player_spawn_event &event) {
	player_pos = event.pos;
	player_dir = event.dir;
	light->shadow_props.set_eye_pos(event.pos + glm::vec3(0.0f, 20.0f, 0.0f));

	return 0;
}

template <const size_t N>
int object_world<N>::handle(player_move_event &event) {
	player_pos = event.pos;
	light->shadow_props.set_eye_pos(event.pos + glm::vec3(0.0f, 20.0f, 0.0f));

	return 0;
}

template <const size_t N>
int object_world<N>::handle(player_look_event &event) {
	player_dir = event.dir;

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

template <const size_t N>
void object_world<N>::do_raycast_and_update() {
	hit_results.clear();

	for (size_t i = 0; i < N; i++) {
		if (! state->active[i]) {
			continue;
		}

		phys::real t = raycast_sphere_test(player_pos, player_dir, state->particles[i].pos, state->particles[i].radius);

		if (t >= 0.0_r) {
			hit_results.push_back({
				.i = i,
				.t = t
			});
		}
	}

	if (! hit_results.size()) {
		bool was_selected = state->select_particle(-1);

		if (was_selected) {
			particle_deselect_event deselect_event(state->sphere_meshes);
			custom_bus.fire(deselect_event);
		}

		return;
	}

	std::sort(std::begin(hit_results), std::end(hit_results), [](const hit_result &a, const hit_result &b) {
		return a.t < b.t;
	});

	bool was_selected = state->select_particle(hit_results[0].i);

	if (was_selected) {
		size_t i = hit_results[0].i;

		particle_select_event select_event(state->sphere_meshes, state->particles[i], i);
		custom_bus.fire(select_event);
	}
}
