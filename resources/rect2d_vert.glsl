#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in ivec2 pos;

layout(location = 5) uniform int screen_width;
layout(location = 6) uniform int screen_height;

void main() {
	float x = 2.0 * (pos.x - screen_width / 2.0) / screen_width;
	float y = 2.0 * (screen_height / 2.0 - pos.y) / screen_height;

	gl_Position = vec4(x, y, 0.0, 1.0);
}