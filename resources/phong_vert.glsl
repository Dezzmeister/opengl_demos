#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_mat;

// In view space
out vec3 frag_pos;
out vec3 frag_normal;

void main() {
	gl_Position = projection * view * model * vec4(pos, 1.0);
	frag_pos = vec3(view * model * vec4(pos, 1.0));
	frag_normal = normal_mat * normal;
}
