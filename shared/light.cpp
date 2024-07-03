#include "light.h"

light_properties::light_properties(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) :
	ambient(_ambient),
	diffuse(_diffuse),
	specular(_specular)
{}

light::light(light_type _type, event_buses &_buses) :
	event_listener<draw_event>(&_buses.render, -50),
	type(_type)
{}

void light::add_to_world() {
	event_listener<draw_event>::subscribe();
}

int light::handle(draw_event &event) {
	event.lights.push_back(this);

	return 0;
}
