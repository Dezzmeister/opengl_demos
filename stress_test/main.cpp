#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../shared/shapes.h"
#include "../shared/events.h"
#include "../shared/flashlight.h"
#include "../shared/gdi_plus_context.h"
#include "../shared/hardware_constants.h"
#include "../shared/instanced_mesh.h"
#include "../shared/controllers.h"
#include "../shared/mesh.h"
#include "../shared/phong_color_material.h"
#include "../shared/player.h"
#include "../shared/point_light.h"
#include "../shared/shader_store.h"
#include "../shared/spotlight.h"
#include "../shared/text2d.h"
#include "../shared/texture_store.h"
#include "../shared/util.h"
#include "../shared/world.h"

static const char help_text[] =
"Controls:\n"
"\tMouse to look around\n"
"\tWASD to move\n"
"\tHold LEFT SHIFT to sprint\n"
"\tF to toggle flashlight\n"
"\tR to start measuring performance, R again to stop\n";

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
	{
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
		if (! is_measuring) {
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
		if (event.key == GLFW_KEY_R) {
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

	player pl(buses);
	hardware_constants hw_consts(buses);

	program_start_event program_start{ window };
	pre_render_pass_event pre_render_event(window, &hw_consts);
	shader_store shaders(buses);
	texture_store textures(buses);
	text2d_renderer text2d(buses);
	draw_event draw_event_inst(window, shaders, textures);
	post_processing_event post_processing_event_inst(
		window,
		shaders,
		textures,
		text2d
	);
	post_render_pass_event post_render_event;

	key_controller keys(buses, {
		GLFW_KEY_W,
		GLFW_KEY_A,
		GLFW_KEY_S,
		GLFW_KEY_D,
		GLFW_KEY_LEFT_SHIFT,
		GLFW_KEY_F,
		GLFW_KEY_R,
		GLFW_KEY_ESCAPE
	});
	screen_controller screen(buses);

	constexpr float cube_size = 0.5f;
	constexpr float cube_spacing = 0.5f;
	constexpr int cubes_per_axis = 100;
	constexpr float step = cube_size + cube_spacing;
	constexpr float width = step * cubes_per_axis;
	constexpr int total_cubes = cubes_per_axis * cubes_per_axis * cubes_per_axis;

	point_light static_light(
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

	world w(buses, {}, { &static_light });

	buses.lifecycle.fire(program_start);

	shapes::init();

	// Gold
	phong_color_material_properties gold_mtl_props(
		glm::vec3(0.24725, 0.1995, 0.0745),
		glm::vec3(0.75164, 0.60648, 0.22648),
		glm::vec3(0.628281, 0.555802, 0.366065),
		128 * 0.4f
	);

	// Obsidian
	phong_color_material_properties obsidian_mtl_props(
		glm::vec3(0.05375, 0.05, 0.06625),
		glm::vec3(0.18275, 0.17, 0.22525),
		glm::vec3(0.332741, 0.328634, 0.346435),
		128 * 0.3f
	);

	phong_color_material gold_mtl(gold_mtl_props);
	phong_color_material obsidian_mtl(obsidian_mtl_props);

	instanced_mesh gold_cubes(shapes::cube.get(), &gold_mtl, total_cubes / 2);
	instanced_mesh obsidian_cubes(shapes::cube.get(), &obsidian_mtl, total_cubes / 2);

	bool mtl_flag = false;
	int gold_ct = 0;
	int obsidian_ct = 0;

	for (float x = -width / 2; x < width / 2; x += step) {
		for (float y = 0.0f; y < width; y += step) {
			for (float z = 2.0f; z < (width + 2.0f); z += step) {
				instanced_mesh * im = nullptr;
				int ct = 0;

				if (obsidian_ct > gold_ct) {
					im = &gold_cubes;
					ct = gold_ct++;
				} else {
					im = &obsidian_cubes;
					ct = obsidian_ct++;
				}

				im->set_model(ct, glm::translate(glm::identity<glm::mat4>(), glm::vec3(
					x, y, z
				)) * glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.5f)));
			}
		}
	}

	w.add_instanced_mesh(&gold_cubes);
	w.add_instanced_mesh(&obsidian_cubes);

	flashlight lc(buses, pl, w, GLFW_KEY_F);

	debug_instrument instr(buses);

	printf(help_text);

	while (!glfwWindowShouldClose(window)) {
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