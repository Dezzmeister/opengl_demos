#pragma once
#include "../math.h"
#include "../rigid_body.h"

namespace phys {
	struct contact {
		rigid_body * a;
		rigid_body * b;

		vec3 point;
		vec3 normal;
		real penetration;

		contact(
			rigid_body * _a,
			rigid_body * _b,
			const vec3 &_point,
			const vec3 &_normal,
			real _penetration
		);
	};
}
