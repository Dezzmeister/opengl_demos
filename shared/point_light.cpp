#include "point_light.h"

point_light::point_light(
	glm::vec3 _pos,
	light_properties _props,
	attenuation_factors _att_factors
) :
	light(light_type::point),
	pos(_pos),
	props(_props),
	att_factors(_att_factors)
{}

void point_light::prepare_draw(int index, const shader_program &shader) const {
	const int i = (index * light_struct_size);

	shader.set_uniform(lights_loc + i + type_loc, static_cast<int>(type));
	shader.set_uniform(lights_loc + i + pos_loc, pos);
	shader.set_uniform(lights_loc + i + ambient_loc, props.ambient);
	shader.set_uniform(lights_loc + i + diffuse_loc, props.diffuse);
	shader.set_uniform(lights_loc + i + specular_loc, props.specular);
	shader.set_uniform(lights_loc + i + att_c_loc, att_factors.constant);
	shader.set_uniform(lights_loc + i + att_l_loc, att_factors.linear);
	shader.set_uniform(lights_loc + i + att_q_loc, att_factors.quadratic);
}

bool point_light::is_eq(const light &other) const {
	if (type != other.type) {
		return false;
	}

	const point_light &pl = static_cast<const point_light&>(other);

	return (pos == pl.pos) && (props == pl.props) && (att_factors == pl.att_factors);
}
