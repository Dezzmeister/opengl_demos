#include "mesh.h"
#include "shader_store.h"
#include "util.h"

static constexpr util::str_kv_pair<int> uniform_locs[] = {
	{ "model", 9 },
	{ "normal_mat", 12 }
};

mesh::mesh(geometry * _geom, material * _mat) :
	model(glm::identity<glm::mat4>()),
	inv_model(glm::inverse(model)),
	geom(_geom),
	mat(_mat)
{

}

void mesh::draw(draw_event &event, const shader_program &shader) const {
	static constexpr int model_loc = util::find_in_map(uniform_locs, "model");
	static constexpr int normal_mat_loc = util::find_in_map(uniform_locs, "normal_mat");

	shader.set_uniform(model_loc, model);

	glm::mat3 normal_mat = glm::mat3(glm::transpose(inv_model * *event.inv_view));
	shader.set_uniform(normal_mat_loc, normal_mat);
}

void mesh::set_model(const glm::mat4 &_model) {
	model = _model;
	inv_model = glm::inverse(model);
}

const glm::mat4& mesh::get_model() const {
	return model;
}

bool operator<(const mesh &a, const mesh &b) {
	return std::make_tuple(
		a.mat, a.geom,
		a.model[0].x, a.model[0].y, a.model[0].z, a.model[0].w,
		a.model[1].x, a.model[1].y, a.model[1].z, a.model[1].w,
		a.model[2].x, a.model[2].y, a.model[2].z, a.model[2].w,
		a.model[3].x, a.model[3].y, a.model[3].z, a.model[3].w
	) < std::make_tuple(
		b.mat, b.geom,
		b.model[0].x, b.model[0].y, b.model[0].z, b.model[0].w,
		b.model[1].x, b.model[1].y, b.model[1].z, b.model[1].w,
		b.model[2].x, b.model[2].y, b.model[2].z, b.model[2].w,
		b.model[3].x, b.model[3].y, b.model[3].z, b.model[3].w
	);
}

bool operator==(const mesh &a, const mesh &b) {
	return (a.geom == b.geom) && (a.mat == b.mat) && (a.model == b.model);
}
