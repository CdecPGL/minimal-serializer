#pragma once

#include <limits>
#include <boost/mpl/list.hpp>

#include "minimal_serializer/fixed_string.hpp"

// Test using string before C++17
#if BOOST_CXX_VERSION < 202002L
template<std::size_t Length>
using fixed_string_t = minimal_serializer::fixed_string<Length>;
#else
// Test using u8string in C++20
template <std::size_t Length>
using fixed_string_t = minimal_serializer::fixed_u8string<Length>;
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

// Tests for standard types
#if BOOST_VERSION >= 107400
using test_serializable_types = boost::mpl::list<int8_t, uint8_t, int16_t, uint16_t,
												int32_t, uint32_t, int64_t, uint64_t, bool,
												float, double,
												test_enum, test_enum_class,
												simple_struct_member_serialize, simple_struct_global_serialize,
												nested_struct>;
#else
using test_serializable_types = boost::mpl::list<int8_t, uint8_t, int16_t, uint16_t,
	int32_t, uint32_t, int64_t, uint64_t, bool,
	test_enum, test_enum_class,
	simple_struct_member_serialize, simple_struct_global_serialize, nested_struct>;
#endif
