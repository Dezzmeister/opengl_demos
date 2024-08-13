#pragma once
#include <concepts>
#include <iostream>
#include <string>

struct uri {
	std::wstring scheme{};
	std::string media_type{};
	std::wstring loc{};
};

class uri_error : public std::runtime_error {
public:
	const size_t char_pos;

	uri_error(const std::string &message, size_t _char_pos);
};

uri parse_uri(std::wistream &_in);
