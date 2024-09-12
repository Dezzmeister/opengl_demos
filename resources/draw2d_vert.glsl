#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec2 pos;

out vec2 pos_out;

void main() {
	pos_out = pos;
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}