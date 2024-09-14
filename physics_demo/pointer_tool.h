#pragma once
#include "../shared/draw2d.h"
#include "../shared/texture_store.h"
#include "tools.h"

class pointer_tool :
	public tool,
	public event_listener<program_start_event>,
	public event_listener<pre_render_pass_event>,
	public event_listener<post_processing_event>,
	public event_listener<particle_select_event>,
	public event_listener<particle_deselect_event>
{
public:
	pointer_tool(
		event_buses &_buses,
		custom_event_bus &_custom_bus,
		texture_store &_textures,
		world &_mesh_world
	);

	void activate() override;
	void deactivate() override;
	bool is_active() const override;

	int handle(program_start_event &event) override;
	int handle(pre_render_pass_event &event) override;
	int handle(post_processing_event &event) override;
	int handle(particle_select_event &event) override;
	int handle(particle_deselect_event &event) override;

private:
	const phys::particle * selected_particle{ nullptr };
	size_t particle_index{ 0 };
	instanced_mesh * particle_mesh{ nullptr };
	world &mesh_world;
	std::unique_ptr<mesh> selected_particle_mesh;
	const font * debug_font{ nullptr };

	void update_meshes();
};
