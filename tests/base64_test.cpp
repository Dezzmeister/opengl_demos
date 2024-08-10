#include <sstream>
#include "../shared/data_formats/base64.h"
#include "test.h"

namespace {
	std::string to_s(const std::vector<uint8_t> &bytes) {
		std::stringstream out{};

		for (uint8_t byte : bytes) {
			out << (char)byte;
		}

		return out.str();
	}
}

void test::setup_base64_tests() {
	suite("base64", {
		test("Decodes a string without padding", {
			std::wstring in = L"c3RyaW5nIHdpdGhvdXQgcGFkZGluZyEh";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "string without padding!!");
		});

		test("(Stream) Decodes a string without padding", {
			std::wstringstream in(L"c3RyaW5nIHdpdGhvdXQgcGFkZGluZyEh");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "string without padding!!");
		});

		test("Decodes a string with +1 ASCII char and explicit padding", {
			std::wstring in = L"b25lIG1vcmUgY2hhcg==";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "one more char");
		});

		test("(Stream) Decodes a string with +1 ASCII char and explicit padding", {
			std::wstringstream in(L"b25lIG1vcmUgY2hhcg==");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "one more char");
		});

		test("Decodes a string with +2 ASCII chars and explicit padding", {
			std::wstring in = L"dHdvIG1vcmUgY2hhcnM=";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "two more chars");
		});

		test("(Stream) Decodes a string with +2 ASCII chars and explicit padding", {
			std::wstringstream in(L"dHdvIG1vcmUgY2hhcnM=");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "two more chars");
		});

		test("Decodes a string with +1 ASCII char and no padding", {
			std::wstring in = L"b25lIG1vcmUgY2hhcg";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "one more char");
		});

		test("(Stream) Decodes a string with +1 ASCII char and no padding", {
			std::wstringstream in(L"b25lIG1vcmUgY2hhcg");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "one more char");
		});

		test("Decodes a string with +2 ASCII chars and no padding", {
			std::wstring in = L"dHdvIG1vcmUgY2hhcnM";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "two more chars");
		});

		test("(Stream) Decodes a string with +2 ASCII chars and no padding", {
			std::wstringstream in(L"dHdvIG1vcmUgY2hhcnM");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "two more chars");
		});

		test("Decodes an empty string", {
			std::wstring in = L"";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(bytes.size() == 0);
		});

		test("(Stream) Decodes an empty string", {
			std::wstringstream in(L"");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(bytes.size() == 0);
		});

		test("Ignores excessive padding", {
			std::wstring in = L"dGhpcyBzdHJpbmcgZG9lc24ndCBuZWVkIHBhZGRpbmcu============";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "this string doesn't need padding.");
		});

		test("(Stream) Ignores excessive padding", {
			std::wstringstream in(L"dGhpcyBzdHJpbmcgZG9lc24ndCBuZWVkIHBhZGRpbmcu============");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "this string doesn't need padding.");
		});

		test("Decodes an empty ASCII string with padding", {
			std::wstring in = L"===";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(bytes.size() == 0);
		});

		test("(Stream) Decodes an empty ASCII string with padding", {
			std::wstringstream in(L"===");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(bytes.size() == 0);
		});

		test("Decodes a single ASCII character with explicit padding", {
			std::wstring in = L"J===";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "$");
		});

		test("(Stream) Decodes a single ASCII character with explicit padding", {
			std::wstringstream in(L"J===");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "$");
		});

		test("Decodes a single ASCII character with no padding", {
			std::wstring in = L"J";
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "$");
		});

		test("(Stream) Decodes a single ASCII character with no padding", {
			std::wstringstream in(L"J");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "$");
		});

		test("Fails to decode a single invalid base64 character", {
			std::wstring in = L".";

			try {
				std::vector<uint8_t> _ = decode_base64(in);

				fail_msg("Expected base64 decoder to throw");
			} catch (base64_error err) {
				expect(err.char_pos == 0L);
			}
		});

		test("(Stream) Fails to decode a single invalid base64 character", {
			std::wstringstream in(L".");

			try {
				std::vector<uint8_t> _ = decode_base64(in);

				fail_msg("Expected base64 decoder to throw");
			} catch (base64_error err) {
				expect(err.char_pos == 0L);
			}
		});

		test("Fails to decode a string with an invalid base64 character after the padding", {
			std::wstring in = L"aValidBase64StringUpUntilTheNewline=\n";

			try {
				std::vector<uint8_t> _ = decode_base64(in);

				fail_msg("Expected base64 decoder to throw");
			} catch (base64_error err) {
				expect(err.char_pos == 36L);
			}
		});

		test("(Stream) Decodes a string with an invalid base64 character after the padding", {
			std::wstringstream in(L"YVZhbGlkQmFzZTY0U3RyaW5nVXBVbnRpbFRoZU5ld2xpbmU==\n");
			std::vector<uint8_t> bytes = decode_base64(in);

			expect(to_s(bytes) == "aValidBase64StringUpUntilTheNewline");
		});
	});
}