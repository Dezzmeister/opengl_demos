#include "constraint.h"

phys::constraint::constraint(real _stiffness, constraint_type _type) :
	stiffness(_stiffness),
	type(_type)
{}