#include "primitive.h"

phys::primitive::primitive(
	int _type,
	rigid_body * _body,
	const mat4 &_offset
) :
	type(_type),
	body(_body),
	offset(_offset)
{}