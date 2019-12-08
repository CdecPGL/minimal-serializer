/*
The MIT License (MIT)

Copyright (c) 2019 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <sstream>
#include <string>
#include <type_traits>

#include "nameof.hpp"

namespace pgl {
	// Enable enum values to output by ostream
	template <typename Enum, std::enable_if_t<std::is_enum_v<Enum>, int> = 0>
	std::ostream & operator <<(std::ostream & os, const Enum & enum_value) {
		os << nameof::nameof_enum(enum_value);
		return os;
	}

	// Enable std::type_info to output by ostream
	inline std::ostream& operator <<(std::ostream& os, const std::type_info& type_info) {
		os << type_info.name();
		return os;
	}

	inline void generate_string_impl(std::ostringstream&) {}

	template <typename First, typename ... Rest>
	void generate_string_impl(std::ostringstream& oss, First&& first, Rest&& ... rest) {
		using non_cvref_first = std::remove_cv_t<std::remove_reference_t<First>>;
		// char types is recognized as character in ostringstream, so '0' means 'end of string' and '0' in char types will not displayed.
		// To avoid this, cast char types to int before pass it to ostringstream
		if constexpr (std::is_same_v<non_cvref_first, char> || std::is_same_v<non_cvref_first, unsigned char> || std::
			is_same_v<non_cvref_first, signed char>) {
			const int first_non_char = first;
			oss << first_non_char;
		} else {
			oss << std::forward<First>(first);
		}
		generate_string_impl(oss, std::forward<Rest>(rest)...);
	}

	// Convert parameters to string and concatenate them to one string. This is thread safe.
	template <typename ... Params>
	std::string generate_string(Params&& ... params) {
		std::ostringstream oss;
		oss << std::boolalpha;
		generate_string_impl(oss, std::forward<Params>(params)...);
		return oss.str();
	}
}
