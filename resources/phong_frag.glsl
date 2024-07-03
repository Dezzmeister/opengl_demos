#version 330 core

struct material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct light {
	vec3 pos;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 frag_pos;
in vec3 frag_normal;

out vec4 frag_color;

uniform material mat;
uniform light l;
uniform mat4 view;

// Lighting computations are done in view space so that we don't need to know
// the camera pos
void main() {
	vec3 view_pos = vec3(0.0, 0.0, 0.0);
	vec3 view_light_pos = vec3(view * vec4(l.pos, 1.0));

	vec3 norm = normalize(frag_normal);
	vec3 light_dir = normalize(view_light_pos - frag_pos);

	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);

	float diff = max(dot(norm, light_dir), 0.0);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess);

	vec3 ambient = l.ambient * mat.ambient;
	vec3 diffuse = l.diffuse * (diff * mat.diffuse);
	vec3 specular = l.specular * (spec * mat.specular);

	vec3 result = ambient + diffuse + specular;
	frag_color = vec4(result, 1.0);
}