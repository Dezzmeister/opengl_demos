layout(location = 0) in vec3 pos;
#ifdef USE_MAPS
layout(location = 1) in vec2 tex_coords_in;
#endif
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_mat;

out vec3 frag_pos_world;
// In view space
out vec3 frag_pos;
out vec3 frag_normal;

#ifdef USE_MAPS
out vec2 tex_coords;
#endif

void main() {
	gl_Position = projection * view * model * vec4(pos, 1.0);
	frag_pos_world = pos;
	frag_pos = vec3(view * model * vec4(pos, 1.0));
	frag_normal = normal_mat * normal;

#ifdef USE_MAPS
	tex_coords = tex_coords_in;
#endif
}
