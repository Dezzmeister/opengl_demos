#include "texture_store.h"

texture_store::texture_store(event_buses &_buses) : 
	event_listener<program_start_event>(&_buses.lifecycle, -100),
	event_listener<program_stop_event>(&_buses.lifecycle),
	textures()
{
	event_listener<program_start_event>::subscribe();
	event_listener<program_stop_event>::subscribe();
}

int texture_store::handle(program_start_event &event) {
	textures.insert(std::make_pair("wall", texture("../resources/wall.jpg")));
	textures.insert(std::make_pair("container2", texture("../resources/container2.png")));
	textures.insert(std::make_pair("container2_specular", texture("../resources/container2_specular.png")));

	event.textures = this;

	return 0;
}

int texture_store::handle(program_stop_event &event) {
	textures.clear();

	return 0;
}
