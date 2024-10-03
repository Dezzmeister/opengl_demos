#include "math.h"

phys::real phys::dot(const vec3 &a, const vec3 &b) {
	return glm::dot(a, b);
}

phys::vec3 phys::normalize(const vec3 &v) {
	return glm::normalize(v);
}

phys::decomposed_vec3 phys::decompose_vec3(const vec3 &v, const vec3 &n) {
	decomposed_vec3 out{};
	real parallel_mag = phys::dot(v, n);

	out.parallel = parallel_mag * n;
	out.perp = v - out.parallel;

	return out;
}