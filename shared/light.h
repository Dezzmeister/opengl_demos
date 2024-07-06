#pragma once
#include <glm/glm.hpp>
#include <string>
#include "shader_program.h"

struct light_properties {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	light_properties(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);

	friend bool operator==(const light_properties &a, const light_properties &b);
};

struct attenuation_factors {
	float constant;
	float linear;
	float quadratic;

	attenuation_factors(float _constant, float _linear, float _quadratic);

	friend bool operator==(const attenuation_factors &a, const attenuation_factors &b);
};

enum class light_type : int {
	point = 0,
	directional = 1,
	spot = 2
};

class world;

class light {
public:
	const light_type type;

	light(light_type _type);

	virtual void prepare_draw(int index, const shader_program &shader) const = 0;

	friend bool operator==(const light &a, const light &b);

	friend class world;

protected:
	static constexpr int num_lights_loc = 20;
	static constexpr int lights_loc = 21;
	static constexpr int type_loc = 0;
	static constexpr int pos_loc = 1;
	static constexpr int dir_loc = 2;
	static constexpr int ambient_loc = 3;
	static constexpr int diffuse_loc = 4;
	static constexpr int specular_loc = 5;
	static constexpr int att_c_loc = 6;
	static constexpr int att_l_loc = 7;
	static constexpr int att_q_loc = 8;
	static constexpr int inner_cutoff_loc = 9;
	static constexpr int outer_cutoff_loc = 10;
	static constexpr int light_struct_size = 11;

	virtual bool is_eq(const light &other) const = 0;
};

