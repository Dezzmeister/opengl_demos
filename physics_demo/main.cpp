#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include "../shared/shapes.h"
#include "../shared/events.h"
#include "../shared/flashlight.h"
#include "../shared/gdi_plus_context.h"
#include "../shared/controllers.h"
#include "../shared/hardware_constants.h"
#include "../shared/physics/math.h"
#include "../shared/mesh.h"
#include "../shared/phong_color_material.h"
#include "../shared/phong_map_material.h"
#include "../shared/physical_particle_emitter.h"
#include "../shared/player.h"
#include "../shared/directional_light.h"
#include "../shared/shader_store.h"
#include "../shared/spotlight.h"
#include "../shared/texture_store.h"
#include "../shared/util.h"
#include "../shared/world.h"
#include "../shared/physics/particle_contact_generators.h"
#include "custom_events.h"
#include "object_world.h"
#include "spawn_tool.h"

using namespace phys::literals;

std::vector<phys::particle> ps{};
const phys::particle_plane_contact_generator<std::vector<phys::particle>> abcdef(ps, phys::vec3{}, phys::vec3{}, 1.0_r);

using namespace phys::literals;
using namespace std::literals::chrono_literals;

static const char help_text[] = R"(
Controls:
	Mouse to look around
	WASD to move
	Hold LEFT SHIFT to spring
	F to toggle flashlight
	T to toggle the particle spawn tool
		While the tool is active:
		LEFT_MOUSE to spawn a particle
		SCROLL to move the cursor
	P to pause the physics simulation
		While the simulation is paused:
		PERIOD to step forward one frame
)";

static void on_window_resize(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main(int argc, const char * const * const argv) {
	event_buses buses;
	custom_event_bus custom_bus;
	gdi_plus_context gdi_plus;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(800, 600, "Physics Demo", NULL, NULL);
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
	glClearColor(0.7f, 0.7f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	player pl(buses);
	hardware_constants hw_consts(buses);

	program_start_event program_start{
		window
	};
	pre_render_pass_event pre_render_event(window, &hw_consts);
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
		GLFW_KEY_T,
		GLFW_KEY_P,
		GLFW_KEY_PERIOD
	});

	mouse_controller mouse(buses, {
		GLFW_MOUSE_BUTTON_LEFT
	});

	screen_controller screen(buses);

	world w(buses);

	buses.lifecycle.fire(program_start);

	shapes::init();

	object_world<1000> objects(
		buses,
		custom_bus,
		w,
		GLFW_KEY_P,
		GLFW_KEY_PERIOD
	);

	flashlight lc(buses, pl, w, GLFW_KEY_F);

	geometry sphere = shapes::make_sphere(20, 10, true);
	spawn_tool sphere_spawner(
		buses,
		custom_bus,
		GLFW_KEY_T,
		&sphere,
		glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.2f)),
		w
	);

	printf(help_text);

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
