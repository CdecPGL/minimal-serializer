/*
The MIT License (MIT)

Copyright (c) 2019-2021 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <limits>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "minimal_serializer/serializer.hpp"
#include "minimal_serializer/fixed_string.hpp"

// 22 bytes
struct simple_struct_member_serialize final {
	const static size_t size = 22;

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
	const static size_t size = 22;

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
	const static size_t size = 94;

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

BOOST_AUTO_TEST_SUITE(serialize_test)
	// Tests for standard types
	using test_types = boost::mpl::list<int8_t, uint8_t, int16_t, uint16_t,
										int32_t, uint32_t, int64_t, uint64_t, bool,
										test_enum, test_enum_class,
										simple_struct_member_serialize, simple_struct_global_serialize, nested_struct>;

	BOOST_AUTO_TEST_CASE_TEMPLATE(test_member_serialize_not_change, Test, test_types) {
		const auto expected = get_default<Test>();
		const auto actual = expected;
		minimal_serializer::serialize(actual);
		// To deal with class who isn't compatible with ostream, compare directly
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE_TEMPLATE(test_member_serialize_consistency, Test, test_types) {
		const auto expected = get_default<Test>();
		auto data1 = minimal_serializer::serialize(expected);
		auto data2 = minimal_serializer::serialize(expected);
		BOOST_CHECK_EQUAL_COLLECTIONS(data1.begin(), data1.end(), data2.begin(), data2.end());
	}

	BOOST_AUTO_TEST_CASE_TEMPLATE(test_member_serialize_deserialize, Test, test_types) {
		const auto expected = get_default<Test>();
		Test actual{};
		auto data = minimal_serializer::serialize(expected);
		minimal_serializer::deserialize(actual, data);
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE_TEMPLATE(test_member_serialize_size, Test, test_types) {
		constexpr auto size = minimal_serializer::serialized_size_v<Test>;
		BOOST_CHECK_EQUAL(get_size<Test>(), size);
	}

	// Tests for std::array
	BOOST_AUTO_TEST_CASE(test_member_serialize_not_change_array) {
		const std::array<int32_t, 4> expected = {-123, 23, 56, 7};
		const auto actual = expected;
		minimal_serializer::serialize(actual);
		BOOST_TEST(expected == actual, boost::test_tools::per_element());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_consistency_array) {
		const std::array<int32_t, 4> expected = {-123, 23, 56, 7};
		auto data1 = minimal_serializer::serialize(expected);
		auto data2 = minimal_serializer::serialize(expected);
		BOOST_CHECK_EQUAL_COLLECTIONS(data1.begin(), data1.end(), data2.begin(), data2.end());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_deserialize_array) {
		const std::array<int32_t, 4> expected = {-123, 23, 56, 7};
		std::array<int32_t, 4> actual{};
		const auto data = minimal_serializer::serialize(expected);
		minimal_serializer::deserialize(actual, data);
		BOOST_TEST(expected == actual, boost::test_tools::per_element());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_size_array) {
		constexpr auto size = minimal_serializer::serialized_size_v<std::array<int32_t, 4>>;
		BOOST_CHECK_EQUAL(sizeof(int32_t)*4, size);
	}

	// Tests for std::tuple
	BOOST_AUTO_TEST_CASE(test_member_serialize_not_change_tuple) {
		const std::tuple<int32_t, uint16_t, bool, minimal_serializer::fixed_string<8>> expected = {
			-123, 23, true, "aiueo"
		};
		const auto actual = expected;
		minimal_serializer::serialize(actual);
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_consistency_tuple) {
		const std::tuple<int32_t, uint16_t, bool, minimal_serializer::fixed_string<8>> expected = {
			-123, 23, true, "aiueo"
		};
		auto data1 = minimal_serializer::serialize(expected);
		auto data2 = minimal_serializer::serialize(expected);
		BOOST_CHECK_EQUAL_COLLECTIONS(data1.begin(), data1.end(), data2.begin(), data2.end());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_deserialize_tuple) {
		const std::tuple<int32_t, uint16_t, bool, minimal_serializer::fixed_string<8>> expected = {
			-123, 23, true, "aiueo"
		};
		std::tuple<int32_t, uint16_t, bool, minimal_serializer::fixed_string<8>> actual{};
		const auto data = minimal_serializer::serialize(expected);
		minimal_serializer::deserialize(actual, data);
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_size_tuple) {
		constexpr auto size = minimal_serializer::serialized_size_v<std::tuple<
			int32_t, uint16_t, bool, minimal_serializer::fixed_string<8>>>;
		BOOST_CHECK_EQUAL(sizeof(int32_t) + sizeof(uint16_t) + 1 + 8, size);
	}

	// Tests for fixed_string
	BOOST_AUTO_TEST_CASE(test_member_serialize_not_change_fixed_string) {
		const minimal_serializer::fixed_string<16> expected = "abcdABCD";
		const auto actual = expected;
		minimal_serializer::serialize(actual);
		BOOST_CHECK_EQUAL(expected, actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_consistency_fixed_string) {
		const minimal_serializer::fixed_string<16> expected = "abcdABCD";
		auto data1 = minimal_serializer::serialize(expected);
		auto data2 = minimal_serializer::serialize(expected);
		BOOST_CHECK_EQUAL_COLLECTIONS(data1.begin(), data1.end(), data2.begin(), data2.end());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_deserialize_fixed_string) {
		const minimal_serializer::fixed_string<16> expected = "abcdABCD";
		minimal_serializer::fixed_string<16> actual{};
		const auto data = minimal_serializer::serialize(expected);
		minimal_serializer::deserialize(actual, data);
		BOOST_CHECK_EQUAL(expected, actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_size_fixed_string) {
		constexpr auto size = minimal_serializer::serialized_size_v<minimal_serializer::fixed_string<16>>;
		BOOST_CHECK_EQUAL(16, size);
	}

	BOOST_AUTO_TEST_CASE(test_serialize_deserialize_offset) {
		const uint64_t expected = 123456789;
		std::vector<uint8_t> buffer(20);
		minimal_serializer::serialize(expected, buffer, 12);
		uint64_t actual = 0;
		minimal_serializer::deserialize(actual, buffer, 12);
		BOOST_CHECK_EQUAL(expected, actual);
	}

	BOOST_AUTO_TEST_CASE(test_serialize_offset_out_of_range) {
		const uint64_t value = 0;
		std::vector<uint8_t> buffer(20);
		BOOST_CHECK_THROW(minimal_serializer::serialize(value, buffer, 13), minimal_serializer::serialization_error);
	}

	BOOST_AUTO_TEST_CASE(test_deserialize_offset_out_of_range) {
		uint64_t value = 0;
		const std::vector<uint8_t> buffer(20);
		BOOST_CHECK_THROW(minimal_serializer::deserialize(value, buffer, 13), minimal_serializer::serialization_error);
	}

	BOOST_AUTO_TEST_CASE(test_serialize_deserialize_stream) {
		const uint64_t expected = 123456789;
		std::ostringstream ostream(std::ios::binary);
		minimal_serializer::serialize(expected, ostream);

		uint64_t actual = 0;
		std::istringstream istream(std::string(ostream.str().data(), ostream.str().size()), std::ios::binary);
		minimal_serializer::deserialize(actual, istream);

		BOOST_CHECK_EQUAL(expected, actual);
	}

BOOST_AUTO_TEST_SUITE_END()
