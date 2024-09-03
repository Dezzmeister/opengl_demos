#include "math.h"

phys::real phys::literals::operator""_r(long double val) {
	return (phys::real)val;
}