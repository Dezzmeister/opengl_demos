#include "light.h"

light_properties::light_properties(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) :
	ambient(_ambient),
	diffuse(_diffuse),
	specular(_specular)
{}

attenuation_factors::attenuation_factors(float _constant, float _linear, float _quadratic) :
	constant(_constant),
	linear(_linear),
	quadratic(_quadratic)
{}

light::light(light_type _type, event_buses &_buses) :
	event_listener<draw_event>(&_buses.render, -50),
	type(_type)
{}

void light::add_to_world() {
	event_listener<draw_event>::subscribe();
}

void light::remove_from_world() {
	event_listener<draw_event>::unsubscribe();
}

int light::handle(draw_event &event) {
	event.add_light(this);

	return 0;
}
