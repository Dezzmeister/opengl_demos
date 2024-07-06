#pragma once
#include "light.h"

class directional_light : public light {
public:

	directional_light(const glm::vec3 _dir, const light_properties _props);

	void prepare_draw(int index, const shader_program &shader) const override;

protected:

	bool is_eq(const light &other) const override;

private:
	glm::vec3 dir;
	light_properties props;
};
