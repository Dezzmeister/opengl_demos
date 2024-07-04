#pragma once
#include <glm/glm.hpp>
#include "events.h"

struct light_properties {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	light_properties(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
};

struct attenuation_factors {
	float constant;
	float linear;
	float quadratic;

	attenuation_factors(float _constant, float _linear, float _quadratic);
};

enum class light_type : int {
	point = 0,
	directional = 1,
	spot = 2
};

class light : public event_listener<draw_event> {
public:
	const light_type type;

	light(light_type _type, event_buses &_buses);

	virtual void add_to_world();
	virtual void remove_from_world();

	virtual int handle(draw_event &event) override;

	virtual void set_uniforms(const std::string &var_name, const shader_program &shader) const = 0;
};

