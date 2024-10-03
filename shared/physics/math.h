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
	using vec2 = glm::vec<2, real, qual>;
	using mat4 = glm::mat<4, 4, real, qual>;
	using mat2 = glm::mat<2, 2, real, qual>;

	inline constexpr real infinity = std::numeric_limits<real>::infinity();

	real dot(const vec3 &a, const vec3 &b);
	vec3 normalize(const vec3 &v);

	template <typename T>
	inline const auto &to_glm = ident<T>;

	template <typename T>
	inline const auto &to_phys = ident<T>;

	namespace literals {
		constexpr real operator""_r(long double val) {
			return (real)val;
		}
	}

	struct decomposed_vec3 {
		vec3 parallel{};
		vec3 perp{};
	};

	// Decomposes a vector into parallel and perpendicular vectors (with reference to `n`).
	// `n` must be normalized. The resulting vectors will add up to `v`.
	struct decomposed_vec3 decompose_vec3(const vec3 &v, const vec3 &n);
}
