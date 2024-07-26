#include "light.h"

static constexpr int SHADOW_MAP_RESOLUTION = 1024;

static constexpr float depth_map_border_color[] = {
	1.0f, 1.0f, 1.0f, 1.0f
};

light_properties::light_properties(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) :
	ambient(_ambient),
	diffuse(_diffuse),
	specular(_specular)
{}

attenuation_factors::attenuation_factors(float _constant, float _linear, float _quadratic) :
	constant(_constant),
	linear(_linear),
	quadratic(_quadratic)
{}

light::light(light_type _type) :
	type(_type),
	shadow_fbo(0, [](unsigned int _fbo) {
		glDeleteFramebuffers(1, &_fbo);
	}),
	depth_map(0, [](unsigned int _tex) {
		glDeleteTextures(1, &_tex);
	})
{}

void light::set_casts_shadow(bool enabled) {
	if (enabled == casts_shadow()) {
		return;
	}

	if (!enabled) {
		depth_map = 0;
		shadow_fbo = 0;
		return;
	}

	glGenFramebuffers(1, &shadow_fbo);
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT,
		SHADOW_MAP_RESOLUTION,
		SHADOW_MAP_RESOLUTION,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, depth_map_border_color);

	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// TODO: Better errors
		printf("Error: framebuffer is not complete\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void light::prepare_shadow_render_pass() const {
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
	glViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
	glClear(GL_DEPTH_BUFFER_BIT);
}

bool light::casts_shadow() const {
	return shadow_fbo != 0;
}

bool operator==(const light_properties &a, const light_properties &b) {
	return (a.ambient == b.ambient) && (a.diffuse == b.diffuse) && (a.specular == b.specular);
}

bool operator==(const attenuation_factors &a, const attenuation_factors &b) {
	return (a.constant == b.constant) && (a.linear == b.linear) && (a.quadratic == b.quadratic);
}

bool operator==(const light &a, const light &b) {
	return a.is_eq(b);
}
