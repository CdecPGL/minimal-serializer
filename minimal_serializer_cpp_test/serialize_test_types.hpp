/*
The MIT License (MIT)

Copyright (c) 2019 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <limits>
#include <boost/mpl/list.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/static_string/static_string.hpp>

#include "minimal_serializer/fixed_string.hpp"

#ifdef __cpp_char8_t
// Test using u8string in C++20
template <std::size_t Length>
using fixed_string_t = minimal_serializer::fixed_u8string<Length>;
using std_string_t = std::u8string; 
#else
// Test using string before C++17
template<std::size_t Length>
using fixed_string_t = minimal_serializer::fixed_string<Length>;
using std_string_t = std::string;
#endif

#ifdef BOOST_STATIC_STRING_CPP20
template <std::size_t Length>
using boost_static_string_t = boost::static_strings::static_u8string<Length>;
#else
template <std::size_t Length>
using boost_static_string_t = boost::static_strings::static_string<Length>;
#endif

// 22 bytes
struct simple_struct_member_serialize final {
	constexpr static size_t size = 22;

	std::array<int32_t, 5> value1;
	uint16_t value2;

	bool operator==(const simple_struct_member_serialize& other) const {
		return value1 == other.value1 &&
			value2 == other.value2;
	}

	static simple_struct_member_serialize get_default() {
		return {
			{10, 11, 12, 13, 14},
			8777
		};
	}

	using serialize_targets = minimal_serializer::serialize_target_container<
		&simple_struct_member_serialize::value1, &simple_struct_member_serialize::value2>;
};

// 22 bytes
struct simple_struct_global_serialize final {
	constexpr static size_t size = 22;

	std::array<int32_t, 5> value1;
	uint16_t value2;

	bool operator==(const simple_struct_global_serialize& other) const {
		return value1 == other.value1 &&
			value2 == other.value2;
	}

	static simple_struct_global_serialize get_default() {
		return {
			{10, 11, 12, 13, 14},
			8777
		};
	}
};

namespace minimal_serializer {
	template <>
	struct serialize_targets<simple_struct_global_serialize> {
		using type = serialize_target_container<&simple_struct_global_serialize::value1, &
												simple_struct_global_serialize::value2>;
	};
}

// 94 bytes
struct nested_struct final {
	constexpr static size_t size = 94;

	uint8_t value1;
	int64_t value2;
	bool value3;
	std::array<uint32_t, 10> value4;
	simple_struct_member_serialize value5;
	simple_struct_global_serialize value6;

	bool operator==(const nested_struct& other) const {
		return value1 == other.value1 &&
			value2 == other.value2 &&
			value3 == other.value3 &&
			value4 == other.value4 &&
			value5 == other.value5 &&
			value6 == other.value6;
	}

	static nested_struct get_default() {
		return {
			255,
			-345345346,
			true,
			{1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
			{{10, 11, 12, 13, 14}, 8777},
			{{10, 11, 12, 13, 14}, 8777}
		};
	}

	using serialize_targets = minimal_serializer::serialize_target_container<
		&nested_struct::value1, &nested_struct::value2, &nested_struct::value3, &nested_struct::value4, &
		nested_struct::value5, &nested_struct::value6>;
};

enum test_enum { a, b, c };

enum class test_enum_class { x, y, z };

template <typename T>
auto get_default() -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	if constexpr (std::is_same_v<T, bool>) {
		return true;
	}
	else {
		if constexpr (std::is_signed_v<T>) {
			return std::numeric_limits<T>::min() / 10;
		}
		else {
			return std::numeric_limits<T>::max() / 10;
		}
	}
}

template <typename T>
auto get_default() -> std::enable_if_t<std::is_enum_v<T>, T> {
	return static_cast<T>(1);
}

template <typename T>
auto get_default() -> decltype(T::get_default, T{}) {
	return T::get_default();
}

template <typename T>
auto get_size() -> std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, size_t> {
	return sizeof(T);
}

template <typename T>
auto get_size() -> decltype(T::size, size_t{}) {
	return T::size;
}

struct simple_struct_not_serialize_targets_defined final {
	std::array<int32_t, 5> value1;
	uint16_t value2;
};

struct not_trivial_struct_serialize_targets_defined final {
	std::vector<int32_t> value1;
	using serialize_targets = minimal_serializer::serialize_target_container<&simple_struct_member_serialize::value1>;
};

// Non string serializable types to test
#if BOOST_VERSION >= 107400
using test_serializable_non_string_types = boost::mpl::list<
	int8_t, uint8_t, int16_t, uint16_t,
	int32_t, uint32_t, int64_t, uint64_t, bool,
	float, double,
	test_enum, test_enum_class,
	simple_struct_member_serialize, simple_struct_global_serialize,
	nested_struct
>;
#else
using test_serializable_non_string_types = boost::mpl::list<
	int8_t, uint8_t, int16_t, uint16_t,
	int32_t, uint32_t, int64_t, uint64_t, bool,
	test_enum, test_enum_class,
	simple_struct_member_serialize, simple_struct_global_serialize,
	nested_struct
>;
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996) // Avoid deprecation of fixed_string and static_string
#endif
// String serializable types to test
using test_serializable_string_types = boost::mpl::list<
	fixed_string_t<8>,
	boost_static_string_t<8>
>;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Tuple line serializable types to test
using test_serializable_tuple_like_types = boost::mpl::list<
	std::array<int32_t, 4>,
	std::tuple<int32_t, int32_t, int32_t>,
	std::pair<int32_t, int32_t>
>;


// All serializable types to test
using test_serializable_types = boost::mpl::joint_view<
	boost::mpl::joint_view<
		test_serializable_non_string_types,
		test_serializable_string_types
	>, test_serializable_tuple_like_types>;

using test_non_serializable_types = boost::mpl::list<
	std::vector<int32_t>, std::map<int32_t, int32_t>,
	std::string, boost::static_strings::static_u16string<8>,
	boost::static_strings::static_u32string<8>, boost::static_strings::static_wstring<8>,
	simple_struct_not_serialize_targets_defined,
	not_trivial_struct_serialize_targets_defined
>;
