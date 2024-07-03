#include "texture_store.h"

texture_store::texture_store(event_buses &_buses) : 
	event_listener<program_start_event>(&_buses.lifecycle),
	event_listener<program_stop_event>(&_buses.lifecycle),
	textures()
{
	event_listener<program_start_event>::subscribe();
	event_listener<program_stop_event>::subscribe();
}

int texture_store::handle(program_start_event &event) {
	textures.insert(std::make_pair("wall", texture("../resources/wall.jpg")));

	return 0;
}

int texture_store::handle(program_stop_event &event) {
	textures.clear();

	return 0;
}
