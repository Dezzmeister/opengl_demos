layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coords_in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_mat;

out vec3 frag_pos_world;
// In view space
out vec3 frag_pos_view;
out vec3 frag_normal;

#ifdef USE_MAPS
out vec2 tex_coords;
#endif

void main() {
	vec4 world_pos = model * vec4(pos, 1.0);
	vec4 view_pos = view * world_pos;

	gl_Position = projection * view_pos;
	frag_pos_world = vec3(world_pos);
	frag_pos_view = vec3(view_pos);
	frag_normal = normal_mat * normal;

#ifdef USE_MAPS
	tex_coords = tex_coords_in;
#endif
}
