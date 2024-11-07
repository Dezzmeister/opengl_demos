#include "contact.h"

phys::contact::contact(
	rigid_body * _a,
	rigid_body * _b,
	const vec3 &_point,
	const vec3 &_normal,
	real _penetration
) :
	a(_a),
	b(_b),
	point(_point),
	normal(_normal),
	penetration(_penetration)
{}