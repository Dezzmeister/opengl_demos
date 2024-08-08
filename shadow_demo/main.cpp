#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include "../shared/controllers.h"
#include "../shared/directional_light.h"
#include "../shared/events.h"
#include "../shared/flashlight.h"
#include "../shared/gdi_plus_context.h"
#include "../shared/hardware_constants.h"
#include "../shared/phong_color_material.h"
#include "../shared/player.h"
#include "../shared/point_light.h"
#include "../shared/shader_constants.h"
#include "../shared/shader_store.h"
#include "../shared/shapes.h"
#include "../shared/texture_store.h"

phong_color_material obsidian{
	phong_color_material_properties{
		glm::vec3(0.05375, 0.05, 0.06625),
		glm::vec3(0.18275, 0.17, 0.22525),
		glm::vec3(0.332741, 0.328634, 0.346435),
		128 * 0.3f
	}
};

phong_color_material pearl{
	phong_color_material_properties{
		glm::vec3(0.25, 0.20725, 0.20725),
		glm::vec3(1, 0.829, 0.829),
		glm::vec3(0.296648, 0.296648, 0.296648),
		128 * 0.088f
	}
};

phong_color_material white_rubber{
	phong_color_material_properties{
		glm::vec3(0.05, 0.05, 0.05),
		glm::vec3(0.5, 0.5, 0.5),
		glm::vec3(0.7, 0.7, 0.7),
		128 * 0.078125f
	}
};

static const glm::vec3 sun_axis(0.0f, 0.0f, 1.0f);
static const glm::vec3 sun_dir(0.0f, -1.0f, 0.0f);
static const float EPSILON = 1e-6f;

struct sun_mover : 
	public event_listener<keydown_event>,
	public event_listener<keyup_event>,
	public event_listener<pre_render_pass_event>
{
private:
	directional_light &sun;
	const mesh &box;
	float angle;
	float d_angle;

public:

	sun_mover(event_buses &_buses, directional_light &_sun, const mesh &_box, float _angle) :
		event_listener<keydown_event>(&_buses.input),
		event_listener<keyup_event>(&_buses.input),
		event_listener<pre_render_pass_event>(&_buses.render),
		sun(_sun),
		box(_box),
		angle(_angle),
		d_angle(0.0f)
	{
		event_listener<keydown_event>::subscribe();
		event_listener<keyup_event>::subscribe();
		event_listener<pre_render_pass_event>::subscribe();
	}

	int handle(keydown_event &event) override {
		if (event.key == GLFW_KEY_I) {
			d_angle -= 0.01f;
		} else if (event.key == GLFW_KEY_O) {
			d_angle += 0.01f;
		}

		return 0;
	}

	int handle(keyup_event &event) override {
		if (event.key == GLFW_KEY_I) {
			d_angle += 0.01f;
		} else if (event.key == GLFW_KEY_O) {
			d_angle -= 0.01f;
		} else {
			return 0;
		}

		const float abs_d_angle = fabs(d_angle);

		if (abs_d_angle < EPSILON || abs_d_angle > (0.01 + EPSILON)) {
			d_angle = 0.0f;
		}

		return 0;
	}

	int handle(pre_render_pass_event &event) override {
		angle += d_angle;

		glm::quat rot(cosf(angle / 2.0f), sun_axis * sinf(angle / 2.0f));
		glm::vec3 new_dir = glm::normalize(rot * sun_dir);
		glm::vec3 box_pos(box.get_model()[3]);
		glm::vec3 new_eye_pos = box_pos - (new_dir * 1.5f);

		sun.set_dir(new_dir);
		sun.shadow_props.set_eye_pos(new_eye_pos);

		return 0;
	}
};

static const glm::vec3 box_axis(0.0f, 1.0f, 0.0f);

struct box_mover :
	public event_listener<keydown_event>,
	public event_listener<keyup_event>,
	public event_listener<pre_render_pass_event>
{
private:
	mesh &box;
	glm::mat4 base_box_model;
	float y;
	float dy;
	float angle;
	float d_angle;

public:

	box_mover(event_buses &_buses, mesh &_box, float _y) :
		event_listener<keydown_event>(&_buses.input),
		event_listener<keyup_event>(&_buses.input),
		event_listener<pre_render_pass_event>(&_buses.render),
		box(_box),
		base_box_model(box.get_model()),
		y(_y),
		dy(0.0f),
		angle(0.0f),
		d_angle(0.0f)
	{
		event_listener<keydown_event>::subscribe();
		event_listener<keyup_event>::subscribe();
		event_listener<pre_render_pass_event>::subscribe();
	}

	int handle(keydown_event &event) override {
		if (event.key == GLFW_KEY_UP) {
			dy += 0.02f;
		} else if (event.key == GLFW_KEY_DOWN) {
			dy -= 0.02f;
		} else if (event.key == GLFW_KEY_LEFT) {
			d_angle += 0.02f;
		} else if (event.key == GLFW_KEY_RIGHT) {
			d_angle -= 0.02f;
		}

		return 0;
	}

	int handle(keyup_event &event) override {
		if (event.key == GLFW_KEY_UP) {
			dy -= 0.02f;
		} else if (event.key == GLFW_KEY_DOWN) {
			dy += 0.02f;
		} else if (event.key == GLFW_KEY_LEFT) {
			d_angle -= 0.02f;
		} else if (event.key == GLFW_KEY_RIGHT) {
			d_angle += 0.02f;
		} else {
			return 0;
		}

		const float abs_dy = fabs(dy);

		if (abs_dy < EPSILON || abs_dy > (0.02f + EPSILON)) {
			dy = 0.0f;
		}

		const float abs_d_angle = fabs(d_angle);

		if (abs_d_angle < EPSILON || abs_d_angle >(0.02f + EPSILON)) {
			d_angle = 0.0f;
		}

		return 0;
	}

	int handle(pre_render_pass_event &event) override {
		if (dy == 0.0f && d_angle == 0.0f) {
			return 0;
		}

		y += dy;
		angle += d_angle;

		glm::mat4 trans = glm::translate(
			glm::identity<glm::mat4>(),
			glm::vec3(0.0f, y, 0.0f)
		);

		glm::mat4 rot = glm::rotate(
			glm::identity<glm::mat4>(),
			angle,
			box_axis
		);

		box.set_model(base_box_model * trans * rot);

		return 0;
	}
};

// This utility allows the user to press a key to cycle through each light in the
// scene. The selected light's shadow depth map will be rendered in the lower left
// corner of the screen. If no light is selected, no depth map will be rendered.
struct debug_instrument :
	public event_listener<keydown_event>,
	public event_listener<post_processing_event>,
	public event_listener<pre_render_pass_event>
{
private:
	static const inline glm::vec3 u_filters[6] = {
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 1),
		glm::vec3(-1, 0, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, 0, -1)
	};
	static const inline glm::vec3 v_filters[6] = {
		glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(-1, 0, 0)
	};

	event_buses &buses;
	const std::vector<light *> &lights;
	int screen_width;
	int screen_height;
	int curr_light;
	int curr_face;
	short debug_key;

public:

	debug_instrument(event_buses &_buses, world &_w, short _debug_key) :
		event_listener<keydown_event>(&_buses.input),
		event_listener<post_processing_event>(&_buses.render),
		event_listener<pre_render_pass_event>(&_buses.render),
		buses(_buses),
		lights(_w.get_lights()),
		screen_width(0),
		screen_height(0),
		curr_light(-1),
		curr_face(-1),
		debug_key(_debug_key)
	{
		event_listener<keydown_event>::subscribe();
		event_listener<post_processing_event>::subscribe();
		event_listener<pre_render_pass_event>::subscribe();
	}

	int handle(keydown_event &event) override {
		if (event.key == debug_key) {
			if (curr_light != -1) {
				if (curr_face >= 5) {
					curr_face = -1;
				} else {
					curr_face++;
					return 0;
				}
			}

			if (curr_light + 1 >= lights.size()) {
				curr_light = -1;
			} else {
				curr_light++;

				const light * l = lights.at(curr_light);

				if (l->type == light_type::point) {
					curr_face++;
				}
			}
		}

		return 0;
	}

	int handle(post_processing_event &event) override {
		if (curr_light >= lights.size()) {
			curr_light = -1;
		}

		if (curr_light == -1) {
			return 0;
		}

		const light * l = lights.at(curr_light);

		if (curr_face == -1) {
			const shader_program &tex_sampler = event.shaders.shaders.at("tex_sampler");

			tex_sampler.use();

			shader_use_event shader_event(tex_sampler);
			buses.render.fire(shader_event);

			constexpr static int tex_loc = util::find_in_map(constants::shader_locs, "tex_sampler_tex");

			if (l->type == light_type::directional) {
				const directional_light * dl = static_cast<const directional_light *>(l);
				unsigned int tex_id = dl->get_depth_map_id();

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex_id);
				tex_sampler.set_uniform(tex_loc, 0);
			}
		} else if (l->type == light_type::point) {
			const point_light * pl = static_cast<const point_light *>(l);
			unsigned int tex_id = pl->get_depth_cubemap_id();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X + curr_face, tex_id);

			const shader_program &cube_sampler = event.shaders.shaders.at("cube_sampler");

			cube_sampler.use();

			shader_use_event shader_event(cube_sampler);
			buses.render.fire(shader_event);

			constexpr static int tex_loc = util::find_in_map(constants::shader_locs, "cube_sampler_cubemap");
			constexpr static int u_filter_loc = util::find_in_map(constants::shader_locs, "cube_sampler_u_filter");
			constexpr static int v_filter_loc = util::find_in_map(constants::shader_locs, "cube_sampler_v_filter");

			cube_sampler.set_uniform(tex_loc, 0);
			cube_sampler.set_uniform(u_filter_loc, u_filters[curr_face]);
			cube_sampler.set_uniform(v_filter_loc, v_filters[curr_face]);
		}

		glViewport(0, 0, 256, 256);
		glClear(GL_DEPTH_BUFFER_BIT);

		shapes::plane->prepare_draw();
		shapes::plane->draw();

		return 0;
	}

	int handle(pre_render_pass_event &event) override {
		screen_width = event.screen_width;
		screen_height = event.screen_height;

		return 0;
	}
};

int main(int argc, const char * const * const argv) {
	event_buses buses;
	gdi_plus_context gdi_plus;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(800, 600, "Shadow Demo", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glClearColor(0.1f, 0.01f, 0.1f, 0.0f);

	hardware_constants hardware_consts(buses);

	player pl(buses);

	program_start_event program_start{
		window
	};
	pre_render_pass_event pre_render_event(window, &hardware_consts);
	shader_store shaders(buses);
	texture_store textures(buses);
	draw_event draw_event_inst(window, shaders, textures);
	post_processing_event post_processing_event_inst(window, shaders, textures);
	post_render_pass_event post_render_event;

	key_controller keys(buses, {
		GLFW_KEY_W,
		GLFW_KEY_A,
		GLFW_KEY_S,
		GLFW_KEY_D,
		GLFW_KEY_LEFT_SHIFT,
		GLFW_KEY_F,
		GLFW_KEY_ESCAPE,
		GLFW_KEY_I,
		GLFW_KEY_O,
		GLFW_KEY_LEFT,
		GLFW_KEY_RIGHT,
		GLFW_KEY_UP,
		GLFW_KEY_DOWN,
		GLFW_KEY_P
	});
	screen_controller screen(buses);

	world w(buses);

	buses.lifecycle.fire(program_start);

	shapes::init();

	mesh floor(shapes::cube.get(), &white_rubber);
	floor.set_model(
		glm::translate(
			glm::identity<glm::mat4>(),
			glm::vec3(0.0f, -1.0f, 0.0f)
		) *
		glm::scale(
			glm::identity<glm::mat4>(),
			glm::vec3(50.0f, 1.0f, 50.0f)
		)
	);

	mesh box(shapes::cube.get(), &obsidian);
	box.set_model(
		glm::translate(
			glm::identity<glm::mat4>(),
			glm::vec3(0.0f, 0.0f, 4.0f)
		) *
		glm::scale(
			glm::identity<glm::mat4>(),
			glm::vec3(0.25f)
		)
	);

	w.add_mesh(&floor);
	w.add_mesh(&box);

	directional_light sun(
		glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)),
		light_properties(
			glm::vec3(0.7f),
			glm::vec3(0.9f),
			glm::vec3(1.0f)
		),
		directional_shadow_caster_properties(
			glm::vec3(0.0f, 1.0f, 4.0f),
			0.5f,
			50.0f,
			1024
		)
	);

	sun.set_casts_shadow(true);
	w.add_light(&sun);

	point_light static_light(
		glm::vec3(1.0f, 1.0f, 4.0f),
		light_properties(
			glm::vec3(0.1f),
			glm::vec3(0.7f),
			glm::vec3(0.8f)
		),
		attenuation_factors(
			1.0f,
			0.027f,
			0.0028f
		),
		point_shadow_caster_properties(
			1024,
			0.1f,
			16.0f
		)
	);

	static_light.set_casts_shadow(true);

	w.add_light(&static_light);

	flashlight lc(buses, pl, w, GLFW_KEY_F);
	sun_mover sun_controller(buses, sun, box, 0.0f);
	box_mover box_controller(buses, box, 0.0f);
	debug_instrument debug_controller(buses, w, GLFW_KEY_P);

	while (! glfwWindowShouldClose(window)) {
		buses.render.fire(pre_render_event);
		buses.render.fire(draw_event_inst);
		buses.render.fire(post_processing_event_inst);

		glfwSwapBuffers(window);
		glfwPollEvents();

		buses.render.fire(post_render_event);
	}

	program_stop_event program_stop(0);
	buses.lifecycle.fire(program_stop);

	glfwTerminate();
	return 0;
}