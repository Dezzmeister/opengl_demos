#pragma once
#include "light.h"

static constexpr float DEFAULT_SHADOW_FRUSTUM_SIZE = 100.0f;
static constexpr float DEFAULT_SHADOW_FRUSTUM_FAR = 1000.0f;

class directional_light;

class shadow_caster_properties {
private:
	glm::vec3 eye_pos;
	glm::vec3 dir;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 view_proj;

	void set_dir(const glm::vec3 &_dir);

	glm::mat4 make_view_mat() const;

	friend class directional_light;

public:

	shadow_caster_properties(
		const glm::vec3 &_dir,
		float _frustum_size,
		float _frustum_far
	);

	void set_eye_pos(const glm::vec3 &_eye_pos);

	const glm::mat4& get_mat() const;
};

static const shadow_caster_properties DEFAULT_SHADOW_CASTER_PROPS(
	glm::vec3(0.0f, 0.0f, -1.0f),
	DEFAULT_SHADOW_FRUSTUM_SIZE,
	DEFAULT_SHADOW_FRUSTUM_FAR
);

class directional_light : public light {
public:
	shadow_caster_properties shadow_props;

	directional_light(
		const glm::vec3 _dir,
		const light_properties _props,
		shadow_caster_properties _shadow_props = DEFAULT_SHADOW_CASTER_PROPS
	);

	void prepare_draw(int index, const shader_program &shader, render_pass_state &render_pass) const override;

	void prepare_draw_shadow_map(const shader_program &shader) const override;

	void set_dir(const glm::vec3 &_dir);

	const glm::vec3& get_dir() const;

protected:

	bool is_eq(const light &other) const override;

private:
	glm::vec3 dir;
	light_properties props;
};
