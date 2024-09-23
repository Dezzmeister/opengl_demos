#include "math.h"

phys::real phys::dot(const phys::vec3 &a, const phys::vec3 &b) {
	return glm::dot(a, b);
}

phys::vec3 phys::normalize(const phys::vec3 &v) {
	return glm::normalize(v);
}