#include "mesh.h"
#include "shader_store.h"

mesh::mesh(event_buses &_buses, geometry &_geom, material &_mat) :
	event_listener<draw_event>(&_buses.render),
	model(glm::identity<glm::mat4>()),
	buses(_buses),
	geom(_geom),
	mat(_mat)
{

}

void mesh::add_to_world() {
	event_listener<draw_event>::subscribe();
}

int mesh::handle(draw_event &event) {
	const std::string &name = mat.shader_name();
	const shader_program &shader = event.shaders.shaders.at(name);

	shader.use();
	mat.draw(event, shader);
	shader.set_uniform("model", model);

	glm::mat3 normal_mat = glm::mat3(glm::transpose(glm::inverse(*event.view * model)));
	shader.set_uniform("normal_mat", normal_mat);

	shader_use_event shader_event(shader);
	buses.render.fire(shader_event);

	geom.draw();

	return 0;
}
