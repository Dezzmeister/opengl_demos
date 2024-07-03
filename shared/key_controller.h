#pragma once
#include <bitset>
#include <GLFW/glfw3.h>
#include "events.h"

class key_controller : public event_listener<pre_render_pass_event> {
public:
	key_controller(event_buses &_buses, std::vector<short> _watched_keys);

	int handle(pre_render_pass_event &event) override;

private:
	event_buses &buses;
	std::bitset<GLFW_KEY_LAST> keys;
	std::vector<short> watched_keys;
};

