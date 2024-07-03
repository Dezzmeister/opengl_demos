#pragma once

namespace util {
	template <typename T, const size_t N>
	constexpr size_t c_arr_size(const T(&)[N]) {
		return N;
	}
};
