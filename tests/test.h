#pragma once
#include <functional>
#include <optional>
#include <source_location>
#include <stdexcept>
#include <string>
#include <unordered_map>

#define test(name, body) test::test_cases[(name)] = []() body

// TODO: Fix these line numbers. None of these are multiline macros; this seems like an MSVC problem
#define expect(cond) if (! (cond)) { throw test::assertion_failure(std::nullopt, __FILE__, __LINE__); } test::noop()
#define expect_msg(msg, cond) if (! (cond)) { throw test::assertion_failure((msg), __FILE__, __LINE__); } test::noop()
#define fail() throw test::forced_failure(std::nullopt, __FILE__, __LINE__)
#define fail_msg(msg) throw test::forced_failure((msg), __FILE__, __LINE__)

namespace test {
	inline std::unordered_map<std::string, std::function<void(void)>> test_cases{};

	struct assertion_failure : public std::runtime_error {
		assertion_failure(std::optional<std::string> message, const char * const file, int line) :
			std::runtime_error("Test assertion failed in " + std::string(file) + " (line " + std::to_string(line) + ")" + (message.has_value() ? ": " + message.value() : ""))
		{}
	};

	struct forced_failure : public std::runtime_error {
		forced_failure(std::optional<std::string> message, const char * const file, int line) :
			std::runtime_error("Forced test failure in " + std::string(file) + " (line " + std::to_string(line) + ")" + (message.has_value() ? ": " + message.value() : ""))
		{}
	};

	inline void noop() {}

	extern void setup_json_parser_tests();
}

