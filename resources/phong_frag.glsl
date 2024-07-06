#extension GL_ARB_explicit_uniform_location : enable

#define MAX_LIGHTS 20

struct material {
#ifdef USE_MAPS
	sampler2D diffuse;
	sampler2D specular;
#else
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
#endif
	float shininess;
};

const int point_light_type = 0;
const int dir_light_type = 1;
const int spotlight_type = 2;

struct light {
	// offset = 0
	int type;

	// offset = 1
	vec3 pos;
	// offset = 2
	vec3 dir;

	// offset = 3
	vec3 ambient;
	// offset = 4
	vec3 diffuse;
	// offset = 5
	vec3 specular;

	// Attenuation factors
	// offset = 6
	float att_c;
	// offset = 7
	float att_l;
	// offset = 8
	float att_q;

	// Cosine of spotlight cutoff angles
	// offset = 9
	float inner_cutoff;
	// offset = 10
	float outer_cutoff;
};

in vec3 frag_pos_world;
in vec3 frag_pos_view;
in vec3 frag_normal;

#ifdef USE_MAPS
in vec2 tex_coords;
#endif

out vec4 frag_color;

layout(location = 10) uniform mat4 view;
layout(location = 13) uniform material mat;

layout(location = 20) uniform int num_lights;
layout(location = 21) uniform light lights[MAX_LIGHTS];


// Lighting computations are done in view space so that we don't need to know
// the camera pos
void main() {
	vec3 color_out = vec3(0.0, 0.0, 0.0);
	vec3 view_pos = vec3(0.0, 0.0, 0.0);
	vec3 norm = normalize(frag_normal);

#ifdef USE_MAPS
	vec3 ambient_color = vec3(texture(mat.diffuse, tex_coords));
	vec3 diffuse_color = ambient_color;
	vec3 specular_color = vec3(texture(mat.specular, tex_coords));
#else
	vec3 ambient_color = mat.ambient;
	vec3 diffuse_color = mat.diffuse;
	vec3 specular_color = mat.specular;
#endif

	for (int i = 0; i < num_lights; i++) {
		light l = lights[i];
		vec3 light_dir;

		if (l.type == point_light_type || l.type == spotlight_type) {
			vec3 view_light_pos = vec3(view * vec4(l.pos, 1.0));

			light_dir = normalize(view_light_pos - frag_pos_view);
		} else if (l.type == dir_light_type) {
			vec3 view_light_dir = vec3(view * vec4(l.dir, 0.0));

			light_dir = normalize(-view_light_dir);
		}

		vec3 view_dir = normalize(view_pos - frag_pos_view);
		vec3 reflect_dir = reflect(-light_dir, norm);

		float diff = max(dot(norm, light_dir), 0.0);
		float spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess);

		vec3 ambient = l.ambient * ambient_color;
		vec3 diffuse = l.diffuse * diff * diffuse_color;
		vec3 specular = l.specular * spec * specular_color;

		if (l.type == spotlight_type) {
			vec3 view_spotlight_dir = vec3(view * vec4(l.dir, 0.0));
			float theta = dot(light_dir, normalize(-view_spotlight_dir));
			float epsilon = l.inner_cutoff - l.outer_cutoff;
			float intensity = clamp((theta - l.outer_cutoff) / epsilon, 0.0, 1.0);

			diffuse *= intensity;
			specular *= intensity;
		}

		if (l.type == point_light_type || l.type == spotlight_type) {
			float d = length(l.pos - frag_pos_world);
			float attenuation = 1.0 / (l.att_c + (l.att_l * d) + (l.att_q * d * d));

			ambient *= attenuation;
			diffuse *= attenuation;
			specular *= attenuation;
		}

		vec3 result = ambient + diffuse + specular;
		color_out += result;
	}

	frag_color = vec4(color_out, 1.0);
}