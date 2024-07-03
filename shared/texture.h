#pragma once
#include "unique_handle.h"

class texture {
public:

	texture(const char * const path);

	unsigned int get_id() const;

private:
	unique_handle<unsigned int> id;
	unsigned int width;
	unsigned int height;
};

