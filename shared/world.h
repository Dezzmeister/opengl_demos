#pragma once
#include "events.h"
#include "instanced_mesh.h"
#include "light.h"
#include "mesh.h"
#include "rendering.h"

class world : 
	public event_listener<draw_event>,
	public event_listener<screen_resize_event>,
	public event_listener<program_start_event>
{
public:
	const static size_t MAX_LIGHTS;

	world(event_buses &_buses, std::vector<mesh *> _meshes = {}, std::vector<light *> _lights = {});

	int handle(draw_event &event) override;

	int handle(screen_resize_event &event) override;

	int handle(program_start_event &event) override;

	// Meshes are sorted in ascending order
	void add_mesh(mesh * m);

	void add_mesh_unsorted(mesh * m);

	void remove_mesh(const mesh * m);

	void add_light(light * l);

	void remove_light(const light * l);

	void add_instanced_mesh(instanced_mesh * _mesh);

	void remove_instanced_mesh(const instanced_mesh * _mesh);

private:
	event_buses &buses;
	std::vector<mesh *> meshes;
	std::vector<light *> lights;
	bool meshes_need_sorting;
	std::vector<instanced_mesh *> instanced_meshes;
	int screen_width;
	int screen_height;

	void prepare_draw_lights(const shader_program &shader, render_pass_state &render_pass) const;

	void prepare_shadow_maps(draw_event &event) const;

	void draw_instanced_meshes(draw_event &event) const;
};
