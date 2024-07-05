#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../shared/shapes.h"
#include "../shared/events.h"
#include "../shared/gdi_plus_context.h"
#include "../shared/key_controller.h"
#include "../shared/mesh.h"
#include "../shared/phong_color_material.h"
#include "../shared/phong_map_material.h"
#include "../shared/player.h"
#include "../shared/point_light.h"
#include "../shared/shader_store.h"
#include "../shared/spotlight.h"
#include "../shared/texture_store.h"
#include "../shared/util.h"

static void on_window_resize(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
}

struct debug_instrument : public event_listener<pre_render_pass_event>, public event_listener<keydown_event> {
	float min_ms{ 1000000 };
	float max_ms{ 0 };
	int num_frames{ 0 };
	float total_ms{ 0 };
	bool is_measuring{ false };

	debug_instrument(event_buses &_buses) :
		event_listener<pre_render_pass_event>(&_buses.render, -30),
		event_listener<keydown_event>(&_buses.input)
	{}

	void add_to_world() {
		event_listener<pre_render_pass_event>::subscribe();
		event_listener<keydown_event>::subscribe();
	}

	void reset() {
		min_ms = 1000000;
		max_ms = 0;
		num_frames = 0;
		total_ms = 0;
	}

	int handle(pre_render_pass_event &event) override {
		if (!is_measuring) {
			return 0;
		}

		const float delta = (float)event.delta.count();

		if (delta < min_ms) {
			min_ms = delta;
		}

		if (delta > max_ms) {
			max_ms = delta;
		}

		total_ms += delta;
		num_frames++;

		return 0;
	}

	int handle(keydown_event &event) override {
		if (event.key == GLFW_KEY_F) {
			is_measuring = !is_measuring;

			if (is_measuring) {
				reset();
			} else {
				const float avg_ms = total_ms / num_frames;
				const float avg_fps = (num_frames / total_ms) * 1000;

				printf("Measured for %f ms\n", total_ms);
				printf("\tMin: %f ms\n", min_ms);
				printf("\tAvg: %f ms\n", avg_ms);
				printf("\tMax: %f ms\n", max_ms);
				printf("\tNo. of Frames: %d\n", num_frames);
				printf("\tAvg. FPS: %f\n", avg_fps);
			}
		}

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

	GLFWwindow * window = glfwCreateWindow(800, 600, "Stress Test", NULL, NULL);
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
	glfwSetFramebufferSizeCallback(window, on_window_resize);
	glClearColor(0.1f, 0.01f, 0.1f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	player pl(buses);

	program_start_event program_start;
	pre_render_pass_event pre_render_event(window);
	shader_store shaders(buses);
	texture_store textures(buses);
	draw_event draw_event_inst(window, shaders, textures, nullptr);
	post_render_pass_event post_render_event;

	key_controller keys(buses, {
		GLFW_KEY_W,
		GLFW_KEY_A,
		GLFW_KEY_S,
		GLFW_KEY_D,
		GLFW_KEY_LEFT_SHIFT,
		GLFW_KEY_F,
		GLFW_KEY_ESCAPE
	});

	buses.lifecycle.fire(program_start);

	shapes::init();

	// Gold
	phong_color_material_properties gold_mtl_props(
		glm::vec3(0.24725, 0.1995, 0.0745),
		glm::vec3(0.75164, 0.60648, 0.22648),
		glm::vec3(0.628281, 0.555802, 0.366065),
		128 * 0.4f
	);

	phong_color_material gold_mtl(gold_mtl_props);
	std::vector<mesh> cubes;

	constexpr float cube_size = 0.5f;
	constexpr float cube_spacing = 0.5f;
	constexpr int cubes_per_axis = 14;
	constexpr float step = cube_size + cube_spacing;
	constexpr float width = step * cubes_per_axis;

	for (float x = -width / 2; x < width / 2; x += step) {
		for (float y = 0.0f; y < width; y += step) {
			for (float z = 2.0f; z < (width + 2.0f); z += step) {
				mesh c(buses, *shapes::cube, gold_mtl);

				c.model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(
					x, y, z
				)) * glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.5f));
				c.add_to_world();
				cubes.push_back(c);
			}
		}
	}

	point_light light(
		buses,
		glm::vec3(0, width / 2, 0),
		light_properties(
			glm::vec3(1.0f),
			glm::vec3(1.0f),
			glm::vec3(1.0f)
		),
		attenuation_factors(
			1.0f,
			0.027f,
			0.0028f
		)
	);
	light.add_to_world();

	debug_instrument instr(buses);
	instr.add_to_world();
	printf("Press 'f' to start measuring and 'f' again to stop\n");

	while (!glfwWindowShouldClose(window)) {
		buses.render.fire(pre_render_event);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		buses.render.fire(draw_event_inst);

		glfwSwapBuffers(window);
		glfwPollEvents();

		buses.render.fire(post_render_event);
	}

	program_stop_event program_stop(0);
	buses.lifecycle.fire(program_stop);

	glfwTerminate();

	return 0;
}