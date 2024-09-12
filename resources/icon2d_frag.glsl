#extension GL_ARB_explicit_uniform_location : enable

in vec2 pos;

out vec4 color;

layout(location = 11) uniform sampler2D icon;

void main() {
	color = texture(icon, pos);
}