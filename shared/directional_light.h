#pragma once
#include "light.h"

static constexpr float DEFAULT_POV_FRUSTUM_SIZE = 100.0f;
static constexpr float DEFAULT_POV_FRUSTUM_FAR = 1000.0f;

class directional_light : public light {
public:

	directional_light(
		const glm::vec3 _dir,
		const light_properties _props,
		const float _pov_frustum_size = DEFAULT_POV_FRUSTUM_SIZE,
		const float _pov_frustum_far = DEFAULT_POV_FRUSTUM_FAR,
		const float _pov_eye_zoom = 2.0f,
		const glm::vec3 _pov_eye_displacement = glm::vec3(0.0)
	);

	void prepare_draw(int index, const shader_program &shader, render_pass_state &render_pass) const override;

	void prepare_draw_shadow_map(const shader_program &shader) const override;

	void set_dir(const glm::vec3 &_dir);

protected:

	bool is_eq(const light &other) const override;

private:
	glm::vec3 dir;
	light_properties props;
	float pov_eye_zoom;
	glm::vec3 pov_eye_displacement;
	glm::mat4 pov_proj;
	glm::mat4 pov_view;
	glm::mat4 pov_view_proj;
};
