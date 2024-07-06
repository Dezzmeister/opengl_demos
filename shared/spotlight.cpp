#include "spotlight.h"

spotlight::spotlight(
	const glm::vec3 _pos,
	const glm::vec3 _dir,
	float _inner_cutoff,
	float _outer_cutoff,
	const light_properties _props,
	const attenuation_factors _att_factors
) :
	light::light(light_type::spot),
	pos(_pos),
	dir(_dir),
	cos_inner_cutoff(cosf(_inner_cutoff)),
	cos_outer_cutoff(cosf(_outer_cutoff)),
	props(_props),
	att_factors(_att_factors)
{}

void spotlight::prepare_draw(int index, const shader_program &shader) const {
	const int i = (index * light_struct_size);

	shader.set_uniform(lights_loc + i + type_loc, static_cast<int>(type));
	shader.set_uniform(lights_loc + i + pos_loc, pos);
	shader.set_uniform(lights_loc + i + dir_loc, dir);
	shader.set_uniform(lights_loc + i + ambient_loc, props.ambient);
	shader.set_uniform(lights_loc + i + diffuse_loc, props.diffuse);
	shader.set_uniform(lights_loc + i + specular_loc, props.specular);
	shader.set_uniform(lights_loc + i + inner_cutoff_loc, cos_inner_cutoff);
	shader.set_uniform(lights_loc + i + outer_cutoff_loc, cos_outer_cutoff);
	shader.set_uniform(lights_loc + i + att_c_loc, att_factors.constant);
	shader.set_uniform(lights_loc + i + att_l_loc, att_factors.linear);
	shader.set_uniform(lights_loc + i + att_q_loc, att_factors.quadratic);
}

bool spotlight::is_eq(const light &other) const {
	if (type != other.type) {
		return false;
	}

	const spotlight &sl = static_cast<const spotlight&>(other);

	return
		(pos == sl.pos) &&
		(dir == sl.dir) &&
		(cos_inner_cutoff == sl.cos_inner_cutoff) &&
		(cos_outer_cutoff == sl.cos_outer_cutoff) &&
		(props == sl.props) &&
		(att_factors == sl.att_factors);
}
