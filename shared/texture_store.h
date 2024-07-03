#pragma once
#include <string>
#include <unordered_map>
#include "events.h"
#include "texture.h"

class texture_store : public event_listener<program_start_event>, public event_listener<program_stop_event> {
public:
	std::unordered_map<std::string, texture> textures;

	texture_store(event_buses &_buses);

	int handle(program_start_event &event) override;

	int handle(program_stop_event &event) override;
};

