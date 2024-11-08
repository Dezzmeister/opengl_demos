#pragma once
#include <functional>
#include <string>
#include <vector>
#include "setup.h"

#define expect(arg) test::expect_impl((arg), __FILE__, __LINE__)

namespace test {
	template <typename T>
	class invertible {
	public:
		T& naht();

	protected:
		bool is_inverted{};
	};

	template <typename ActualType>
	class matchers {};

	template <typename T>
	concept numeric = std::integral<T> || std::floating_point<T>;

	template <numeric ActualType>
	class matchers<ActualType> : public invertible<matchers<ActualType>> {
	public:
		matchers(const ActualType _actual, const char * const _file, int _line);

		void to_be(const ActualType expected) const;

	private:
		const ActualType actual;
		const char * const file;
		const int line;
	};

	template <>
	class matchers<std::string> : public invertible<matchers<std::string>> {
	public:
		matchers(const std::string &_actual, const char * const _file, int _line);

		void to_be(const std::string &expected) const;
		void to_contain(const std::string &substring) const;
		void to_have_size(size_t size) const;

	private:
		const std::string actual;
		const char * const file;
		const int line;
	};

	template <>
	class matchers<std::wstring> : public invertible<matchers<std::wstring>> {
	public:
		matchers(const std::wstring &_actual, const char * const _file, int _line);

		void to_be(const std::wstring &expected) const;
		void to_contain(const std::wstring &substring) const;
		void to_have_size(size_t size) const;

	private:
		const std::wstring actual;
		const char * const file;
		const int line;
	};

	template <typename T>
	class matchers<std::vector<T>> : public invertible<matchers<std::vector<T>>> {
	public:
		matchers(const std::vector<T> &_actual, const char * const _file, int _line);

		void to_have_item(const T &item, const std::string &item_name) const;
		void to_have_size(size_t size) const;

	private:
		const std::vector<T> &actual;
		const char * const file;
		const int line;
	};

	template <typename Input>
	matchers<Input> expect_impl(const Input &actual, const char * const _file, int _line);
}

template <typename T>
T& test::invertible<T>::naht() {
	is_inverted = ! is_inverted;

	return *static_cast<T*>(this);
}

template <test::numeric ActualType>
test::matchers<ActualType>::matchers(
	const ActualType _actual,
	const char * const _file,
	int _line
) :
	invertible<matchers<ActualType>>(),
	actual(_actual),
	file(_file),
	line(_line)
{}

template <test::numeric ActualType>
void test::matchers<ActualType>::to_be(const ActualType expected) const {
	bool is_match = actual == expected;

	if (this->is_inverted && is_match) {
		throw assertion_failure(
			"Expected " + std::to_string(actual) + " not to be " + std::to_string(expected),
			file,
			line
		);
	} else if (! this->is_inverted && ! is_match) {
		throw assertion_failure(
			"Expected " + std::to_string(actual) + " to be " + std::to_string(expected),
			file,
			line
		);
	}
}

template <typename T>
test::matchers<std::vector<T>>::matchers(
	const std::vector<T> &_actual,
	const char * const _file,
	int _line
) :
	invertible<matchers<std::vector<T>>>(),
	actual(_actual),
	file(_file),
	line(_line)
{}

template <typename T>
void test::matchers<std::vector<T>>::to_have_item(const T &item, const std::string &item_name) const {
	bool was_found = std::find(std::begin(actual), std::end(actual), item) != std::end(actual);

	if (this->is_inverted && was_found) {
		throw assertion_failure(
			"Expected not to find item \"" + item_name + "\" in vector",
			file,
			line
		);
	} else if (! this->is_inverted && ! was_found) {
		throw assertion_failure(
			"Expected to find item \"" + item_name + "\" in vector",
			file,
			line
		);
	}
}

template <typename T>
void test::matchers<std::vector<T>>::to_have_size(size_t size) const {
	bool has_size = actual.size() == size;

	if (this->is_inverted && has_size) {
		throw assertion_failure(
			"Expected vector not to have size " + std::to_string(size),
			file,
			line
		);
	} else if (! this->is_inverted && ! has_size) {
		throw assertion_failure(
			"Expected vector to have size " + std::to_string(size) +
			", actual size is " + std::to_string(actual.size()),
			file,
			line
		);
	}
}

template <typename Input>
test::matchers<Input> test::expect_impl(const Input &actual, const char * const _file, int _line) {
	return matchers<Input>(actual, _file, _line);
}
