#include <optional>
#include "parsing.h"
#include "uri.h"

namespace {
	size_t parse_pct_encoded(parsing::parser_state &state, std::wstringstream &out) {
		if (state.peek() != L'%') {
			return 0L;
		}

		state.get();

		if (! parsing::parse_hexdig(state, out)) {
			throw uri_error("Expected a hex digit", state.get_col_num());
		}

		if (! parsing::parse_hexdig(state, out)) {
			throw uri_error("Expected a hex digit", state.get_col_num());
		}

		return 3L;
	}

	std::wstring parse_scheme(parsing::parser_state &state) {
		std::wstringstream out{};
		size_t start_pos = state.get_col_num();

		if (! parsing::parse_alpha(state, out)) {
			throw uri_error("Scheme must begin with a letter", state.get_col_num());
		}

		while (
			parsing::parse_alpha(state, out) ||
			parsing::parse_digit(state, out) ||
			parsing::parse_one_char(state, L'+', out) ||
			parsing::parse_one_char(state, L'-', out) ||
			parsing::parse_one_char(state, L'.', out)
		);

		return out.str();
	}

	bool parse_unreserved(parsing::parser_state &state, std::wstringstream &out) {
		return (
			parsing::parse_alpha(state, out) ||
			parsing::parse_digit(state, out) ||
			parsing::parse_one_char(state, L'-', out) ||
			parsing::parse_one_char(state, L'.', out) ||
			parsing::parse_one_char(state, L'_', out) ||
			parsing::parse_one_char(state, L'~', out)
		);
	}

	// Despite the name, the `sub-delims` production will only match one terminal
	bool parse_sub_delims(parsing::parser_state &state, std::wstringstream &out) {
		return (
			parsing::parse_one_char(state, L'!', out) ||
			parsing::parse_one_char(state, L'$', out) ||
			parsing::parse_one_char(state, L'&', out) ||
			parsing::parse_one_char(state, L'\'', out) ||
			parsing::parse_one_char(state, L'(', out) ||
			parsing::parse_one_char(state, L')', out) ||
			parsing::parse_one_char(state, L'*', out) ||
			parsing::parse_one_char(state, L'+', out) ||
			parsing::parse_one_char(state, L',', out) ||
			parsing::parse_one_char(state, L';', out) ||
			parsing::parse_one_char(state, L'=', out)
		);
	}

	// Despite the name, the `gen-delims` production will only match one terminal
	bool parse_gen_delims(parsing::parser_state &state, std::wstringstream &out) {
		return (
			parsing::parse_one_char(state, L':', out) ||
			parsing::parse_one_char(state, L'/', out) ||
			parsing::parse_one_char(state, L'?', out) ||
			parsing::parse_one_char(state, L'#', out) ||
			parsing::parse_one_char(state, L'[', out) ||
			parsing::parse_one_char(state, L']', out) ||
			parsing::parse_one_char(state, L'@', out)
		);
	}

	std::optional<std::wstring> parse_userinfo(parsing::parser_state &state) {
		std::wstringstream out{};

		while (
			parse_unreserved(state, out) ||
			parse_pct_encoded(state, out) ||
			parse_sub_delims(state, out) ||
			parsing::parse_one_char(state, L':', out)
		);

		std::wstring str_out = out.str();

		// TODO: Null sink
		if (! parsing::parse_one_char(state, L'@', out)) {
			return std::nullopt;
		}

		return str_out;
	}

	/*
	uri_hier_part parse_hier_part(parsing::parser_state &state) {

	}
	*/
}

uri_error::uri_error(const std::string &message, size_t _char_pos) :
	std::runtime_error("Error parsing URI (col " + std::to_string(char_pos) + "): " + message),
	char_pos(_char_pos)
{}

uri parse_uri(std::wistream &_in) {
	parsing::parser_state state(_in);

	std::wstring scheme = parse_scheme(state);

	// TODO: The rest of this function
	return {};
}
