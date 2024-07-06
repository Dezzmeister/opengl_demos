#pragma once
#include <stdexcept>

namespace util {
	template <typename T, const size_t N>
	constexpr size_t c_arr_size(const T(&)[N]) {
		return N;
	}

	template <typename Val>
	struct str_kv_pair {
		const char key[256];
		const Val val;
	};

	// This can be used to implement a rudimentary compile-time map. Create a constexpr array
	// of `str_kv_pair` and call this function in a constexpr context 
	template <typename Val, const size_t N, const size_t M>
	constexpr Val find_in_map(const str_kv_pair<Val>(&pairs)[N], const char(&key)[M]) {
		for (size_t i = 0; i < N; i++) {
			bool found = true;

			for (size_t j = 0; j < M; j++) {
				if (pairs[i].key[j] != key[j]) {
					found = false;
					break;
				}
			}

			if (found) {
				return pairs[i].val;
			}
		}

		throw std::logic_error("Key not in map");
	}
};
