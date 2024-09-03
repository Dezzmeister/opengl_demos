#pragma once
#include <glm/glm.hpp>
#include <limits>

namespace {
	constexpr glm::qualifier qual = glm::qualifier::defaultp;

	template <typename T>
	T ident(T t) {
		return t;
	}
}

namespace phys {
	using real = float;
	using vec3 = glm::vec<3, real, qual>;
	using mat4 = glm::mat<4, 4, real, qual>;

	constexpr real infinity = std::numeric_limits<real>::infinity();

	template <typename T>
	const auto &dot = glm::dot<T>;

	template <typename T>
	const auto &to_glm = ident<T>;

	template <typename T>
	const auto &to_phys = ident<T>;

	namespace literals {
		real operator""_r(long double val);
	}
}
