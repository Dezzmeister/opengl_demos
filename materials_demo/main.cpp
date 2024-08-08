#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../shared/shapes.h"
#include "../shared/events.h"
#include "../shared/flashlight.h"
#include "../shared/gdi_plus_context.h"
#include "../shared/controllers.h"
#include "../shared/hardware_constants.h"
#include "../shared/mesh.h"
#include "../shared/phong_color_material.h"
#include "../shared/phong_map_material.h"
#include "../shared/player.h"
#include "../shared/point_light.h"
#include "../shared/shader_store.h"
#include "../shared/spotlight.h"
#include "../shared/texture_store.h"
#include "../shared/util.h"
#include "../shared/world.h"

static const char help_text[] =
"Controls:\n"
"\tMouse to look around\n"
"\tWASD to move\n"
"\tHold LEFT SHIFT to sprint\n"
"\tF to toggle flashlight\n"
"\tArrow keys to move the point light\n";

// Materials taken from http://devernay.free.fr/cours/opengl/materials.html
static phong_color_material mtls[] = {
	// Emerald
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.0215, 0.1745, 0.0215),
			glm::vec3(0.07568, 0.61424, 0.07568),
			glm::vec3(0.633, 0.727811, 0.633),
			128 * 0.6f
		},
	},
	// Jade
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.135, 0.2225, 0.1575),
			glm::vec3(0.54, 0.89, 0.63),
			glm::vec3(0.316228, 0.316228, 0.316228),
			128 * 0.1f
		},
	},
	// Obsidian
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.05375, 0.05, 0.06625),
			glm::vec3(0.18275, 0.17, 0.22525),
			glm::vec3(0.332741, 0.328634, 0.346435),
			128 * 0.3f
		},
	},
	// Pearl
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.25, 0.20725, 0.20725),
			glm::vec3(1, 0.829, 0.829),
			glm::vec3(0.296648, 0.296648, 0.296648),
			128 * 0.088f
		},
	},
	// Ruby
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.1745, 0.01175, 0.01175),
			glm::vec3(0.61424, 0.04136, 0.04136),
			glm::vec3(0.727811, 0.626959, 0.626959),
			128 * 0.6f
		},
	},
	// Turquoise
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.1, 0.18725, 0.1745),
			glm::vec3(0.396, 0.74151, 0.69102),
			glm::vec3(0.297254, 0.30829, 0.306678),
			128 * 0.1f
		},
	},
	// Brass
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.329412, 0.22352, 0.027451),
			glm::vec3(0.780392, 0.568627, 0.113725),
			glm::vec3(0.992157, 0.941176, 0.807843),
			128 * 0.21794872f
		},
	},
	// Bronze
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.2125, 0.1275, 0.054),
			glm::vec3(0.714, 0.4284, 0.18144),
			glm::vec3(0.393548, 0.271906, 0.166721),
			128 * 0.2f
		},
	},
	// Chrome
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.25, 0.25, 0.25),
			glm::vec3(0.4, 0.4, 0.4),
			glm::vec3(0.774597, 0.774597, 0.774597),
			128 * 0.6f
		},
	},
	// Copper
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.19125, 0.0735, 0.0225),
			glm::vec3(0.7038, 0.27048, 0.0828),
			glm::vec3(0.256777, 0.137622, 0.086014),
			128 * 0.1f
		},
	},
	// Gold
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.24725, 0.1995, 0.0745),
			glm::vec3(0.75164, 0.60648, 0.22648),
			glm::vec3(0.628281, 0.555802, 0.366065),
			128 * 0.4f
		},
	},
	// Silver
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.19225, 0.19225, 0.19225),
			glm::vec3(0.50754, 0.50754, 0.50754),
			glm::vec3(0.508273, 0.508273, 0.508273),
			128 * 0.4f
		},
	},
	// Black plastic
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.01, 0.01, 0.01),
			glm::vec3(0.5, 0.5, 0.5),
			128 * 0.25f
		},
	},
	// Cyan plastic
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.0, 0.1, 0.06),
			glm::vec3(0.0, 0.50980392, 0.50980392),
			glm::vec3(0.50196078, 0.50196078, 0.50196078),
			128 * 0.25f
		},
	},
	// Green plastic
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.1, 0.35, 0.1),
			glm::vec3(0.45, 0.55, 0.45),
			128 * 0.25f
		},
	},
	// Red plastic
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.5, 0.0, 0.0),
			glm::vec3(0.7, 0.6, 0.6),
			128 * 0.25f
		},
	},
	// White plastic
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.55, 0.55, 0.55),
			glm::vec3(0.70, 0.70, 0.70),
			128 * 0.25f
		},
	},
	// Yellow plastic
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.5, 0.5, 0.0),
			glm::vec3(0.6, 0.6, 0.5),
			128 * 0.25f
		},
	},
	// Black rubber
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.02, 0.02, 0.02),
			glm::vec3(0.01, 0.01, 0.01),
			glm::vec3(0.4, 0.4, 0.4),
			128 * 0.078125f
		},
	},
	// Cyan rubber
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.0, 0.05, 0.05),
			glm::vec3(0.4, 0.5, 0.5),
			glm::vec3(0.04, 0.7, 0.7),
			128 * 0.078125f
		},
	},
	// Green rubber
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.0, 0.05, 0.0),
			glm::vec3(0.4, 0.5, 0.4),
			glm::vec3(0.04, 0.7, 0.04),
			128 * 0.078125f
		},
	},
	// Red rubber
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.05, 0.0, 0.0),
			glm::vec3(0.5, 0.4, 0.4),
			glm::vec3(0.7, 0.04, 0.04),
			128 * 0.078125f
		},
	},
	// White rubber
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.05, 0.05, 0.05),
			glm::vec3(0.5, 0.5, 0.5),
			glm::vec3(0.7, 0.7, 0.7),
			128 * 0.078125f
		},
	},
	// Yellow rubber
	phong_color_material{
		phong_color_material_properties{
			glm::vec3(0.05, 0.05, 0.0),
			glm::vec3(0.5, 0.5, 0.4),
			glm::vec3(0.7, 0.7, 0.04),
			128 * 0.078125f
		}
	}
};

phong_color_material_properties floor_mtl_props(
	glm::vec3(0.05, 0.05, 0.05),
	glm::vec3(0.5, 0.5, 0.5),
	glm::vec3(0.7, 0.7, 0.7),
	128 * 0.078125f
);
phong_color_material floor_mtl(floor_mtl_props);
phong_map_material wooden_cube_mtl("container2", "container2_specular", 32.0f);

struct object_controller : 
	public event_listener<pre_render_pass_event>, 
	public event_listener<keydown_event>,
	public event_listener<keyup_event>
{
	inline static float light_speed = 0.05f;

	std::unique_ptr<point_light> light{};
	std::vector<std::unique_ptr<mesh>> cubes{};
	std::unique_ptr<mesh> floor{};
	std::unique_ptr<mesh> wooden_cube{};
	glm::vec3 light_motion{ 0.0f };

	object_controller(event_buses &_buses, world &w) :
		event_listener<pre_render_pass_event>(&_buses.render),
		event_listener<keydown_event>(&_buses.input),
		event_listener<keyup_event>(&_buses.input)
	{
		event_listener<pre_render_pass_event>::subscribe();
		event_listener<keydown_event>::subscribe();
		event_listener<keyup_event>::subscribe();

		const size_t num_materials = util::c_arr_size(mtls);
		const float cube_size = 0.5f;
		const float spacing = 0.5f;
		const float width = (cube_size + spacing) * (num_materials - 1);

		for (size_t i = 0; i < num_materials; i++) {
			float pos = (i * (cube_size + spacing)) - (width / 2);

			std::unique_ptr<mesh> cube = std::make_unique<mesh>(shapes::cube.get(), mtls + i);
			cube->set_model(glm::translate(glm::identity<glm::mat4>(), glm::vec3(
				pos,
				0.0f,
				4.0f
			)) * glm::scale(glm::identity<glm::mat4>(), glm::vec3(cube_size)));

			w.add_mesh(cube.get());

			cubes.push_back(std::move(cube));
		}

		light = std::make_unique<point_light>(
			glm::vec3(0.0f, 0.0f, 4.0f - width),
			light_properties(
				glm::vec3(1.0f),
				glm::vec3(1.0f),
				glm::vec3(1.0f)
			),
			attenuation_factors(
				1.0f,
				0.027f,
				0.0028f
			),
			point_shadow_caster_properties(
				1024,
				0.1f,
				100.0f
			)
		);

		light->set_casts_shadow(true);

		w.add_light(light.get());

		floor = std::make_unique<mesh>(shapes::plane.get(), &floor_mtl);
		floor->set_model(glm::translate(glm::identity<glm::mat4>(), glm::vec3(
			0.0f,
			-1.0f,
			0.0f
		)) * glm::scale(glm::identity<glm::mat4>(), glm::vec3(500.0f, 1.0f, 500.0f)));

		w.add_mesh(floor.get());

		wooden_cube = std::make_unique<mesh>(shapes::cube.get(), &wooden_cube_mtl);
		wooden_cube->set_model(glm::translate(glm::identity<glm::mat4>(), glm::vec3(
			-4.0f,
			0.0f,
			0.0f
		)) * glm::scale(glm::identity<glm::mat4>(), glm::vec3(cube_size)));

		w.add_mesh(wooden_cube.get());
	}

	int handle(pre_render_pass_event &event) override {
		if (glm::length(light_motion) < 1e-6) {
			return 0;
		}

		glm::vec3 scaled_motion = light_motion * light_speed / glm::length(light_motion);

		light->set_pos(light->get_pos() + scaled_motion);

		return 0;
	}

	int handle(keydown_event &event) override {
		if (event.key == GLFW_KEY_LEFT) {
			light_motion.x += 1.0f;
		} else if (event.key == GLFW_KEY_RIGHT) {
			light_motion.x -= 1.0f;
		} else if (event.key == GLFW_KEY_UP) {
			light_motion.z += 1.0f;
		} else if (event.key == GLFW_KEY_DOWN) {
			light_motion.z -= 1.0f;
		}

		return 0;
	}

	int handle(keyup_event &event) override {
		if (event.key == GLFW_KEY_LEFT) {
			light_motion.x -= 1.0f;
		} else if (event.key == GLFW_KEY_RIGHT) {
			light_motion.x += 1.0f;
		} else if (event.key == GLFW_KEY_UP) {
			light_motion.z -= 1.0f;
		} else if (event.key == GLFW_KEY_DOWN) {
			light_motion.z += 1.0f;
		}

		return 0;
	}
};

static void on_window_resize(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main(int argc, const char * const * const argv) {
	event_buses buses;
	gdi_plus_context gdi_plus;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(800, 600, "Phong Materials Demo", NULL, NULL);
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
		GLFW_KEY_LEFT,
		GLFW_KEY_RIGHT,
		GLFW_KEY_UP,
		GLFW_KEY_DOWN
	});

	screen_controller screen(buses);

	world w(buses);

	buses.lifecycle.fire(program_start);

	shapes::init();

	object_controller static_objects(buses, w);

	flashlight lc(buses, pl, w, GLFW_KEY_F);

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
