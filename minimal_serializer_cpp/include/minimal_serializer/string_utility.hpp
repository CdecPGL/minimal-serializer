/*
The MIT License (MIT)

Copyright (c) 2019-2022 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <sstream>
#include <string>
#include <type_traits>

#include <boost/config.hpp>

#include "nameof.hpp"

namespace minimal_serializer {
	namespace generate_string_type_traits {
		/**
		 * Char types outputted as integer in generate_string.
		 */
		template<typename T>
		constexpr bool is_char_as_integer_v = std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>;

#if BOOST_CXX_VERSION < 202002L
		/**
		 * Not supported char types in generate_string.
		 */
		template<typename T>
		constexpr bool is_not_supported_char_v = std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>;
#else
		/**
		 * Not supported char types in generate_string.
		 */
		template<typename T>
		constexpr bool is_not_supported_char_v = std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>;
#endif

#if BOOST_CXX_VERSION < 202002L
		/**
		 * Not supported natively by ostream but supported additionally char array types in generate_string.
		 */
		template<typename T>
		constexpr bool is_additionally_supported_char_as_array_v = false;
#else
		/**
		 * Not supported natively by ostream but supported additionally char array types in generate_string.
		 */
		template<typename T>
		constexpr bool is_additionally_supported_char_as_array_v = std::is_same_v<T, char8_t>;
#endif

		/**
		 * Not supported char array types in generate_string.
		 */
		template<typename T>
		constexpr bool is_not_supported_char_array_v = std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>;
	}

	// return true if generating string from enum type is supported. If this value is false, enum type values are treated as its underlying type values. This depends on nameof C++.
	constexpr bool is_generate_enum_string_supported = nameof::is_nameof_enum_supported;

	template <typename T>
	void generate_string_converter(std::ostringstream& oss, T&& value) {
		using namespace generate_string_type_traits;
		using non_cv_ref_t = std::remove_cv_t<std::remove_reference_t<T>>;
		static_assert(!is_not_supported_char_v<non_cv_ref_t>, "Not supported character type.");
		static_assert(!is_not_supported_char_array_v<non_cv_ref_t>, "Not supported character array type.");

		// char types are recognized as character in ostringstream, so '0' means 'end of string' and '0' in char types will not displayed.
		// To avoid these, cast char types assigned to uint8_t and int8_t to int before pass them to ostringstream
		if constexpr (is_char_as_integer_v<non_cv_ref_t>) {
			oss << std::char_traits<non_cv_ref_t>::to_int_type(value);
		}
		else if constexpr (std::is_pointer_v<non_cv_ref_t> && is_additionally_supported_char_as_array_v<std::remove_pointer_t<non_cv_ref_t>>) {
			using element_t = std::remove_pointer_t<non_cv_ref_t>;
			oss << reinterpret_cast<char*>(const_cast<element_t*>(value));
		}
		else if constexpr (std::is_array_v<non_cv_ref_t> && is_additionally_supported_char_as_array_v<std::remove_extent_t<non_cv_ref_t>>) {
			using element_t = std::remove_extent_t<non_cv_ref_t>;
			oss << reinterpret_cast<char*>(const_cast<element_t*>(&value[0]));
		}
		else if constexpr (std::is_enum_v<non_cv_ref_t>) {
			if constexpr (is_generate_enum_string_supported) {
				oss << nameof::nameof_enum(value);
			}
			else {
				oss << static_cast<std::underlying_type_t<non_cv_ref_t>>(value);
			}
		}
#if BOOST_CXX_VERSION >= 202002L
		else if constexpr (std::is_same_v<non_cv_ref_t, std::u8string>) {
			oss << reinterpret_cast<const char*>(const_cast<char*>(value.c_str()));
		}
#endif
		else {
			oss << value;
		}
	}

	template <>
	inline void generate_string_converter(std::ostringstream& oss, const std::type_info& value) {
		oss << value.name();
	}

	inline void generate_string_impl(std::ostringstream&) {}

	template <typename First, typename ... Rest>
	void generate_string_impl(std::ostringstream& oss, First&& first, Rest&& ... rest) {
		generate_string_converter(oss, std::forward<First>(first));
		generate_string_impl(oss, std::forward<Rest>(rest)...);
	}

	/**
	 * Convert parameters to string and concatenate them to one string. This is thread safe.
	 * @note Main use case of this function is log string generation which is outputted to std::ostream. So we dont implement the version to return std::u8string because std::u8string is not supported as std::ostream input in current C++ version (C++20).
	 * @warning This function supports char8_t and std::u8string as parameters but this might not work correctly in the system where char type does not represent UTF-8.
	 */
	template <typename ... Params>
	std::string generate_string(Params&& ... params) {
		std::ostringstream oss;
		oss << std::boolalpha;
		generate_string_impl(oss, std::forward<Params>(params)...);
		return oss.str();
	}
}
