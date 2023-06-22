/*
The MIT License (MIT)

Copyright (c) 2019 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <limits>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "minimal_serializer/serializer.hpp"
#include "minimal_serializer/fixed_string.hpp"

#include "serialize_test_types.hpp"

using namespace std;
using namespace minimal_serializer;

BOOST_AUTO_TEST_SUITE(serialize_test)

	BOOST_AUTO_TEST_CASE_TEMPLATE(test_member_serialize_not_change, Test, test_serializable_non_string_types) {
		const auto expected = get_default<Test>();
		const auto actual = expected;
		minimal_serializer::serialize(actual);
		// To deal with class who isn't compatible with ostream, compare directly
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE_TEMPLATE(test_member_serialize_consistency, Test, test_serializable_non_string_types) {
		const auto expected = get_default<Test>();
		auto data1 = minimal_serializer::serialize(expected);
		auto data2 = minimal_serializer::serialize(expected);
		BOOST_CHECK_EQUAL_COLLECTIONS(data1.begin(), data1.end(), data2.begin(), data2.end());
	}

	BOOST_AUTO_TEST_CASE_TEMPLATE(test_member_serialize_deserialize, Test, test_serializable_non_string_types) {
		const auto expected = get_default<Test>();
		Test actual{};
		auto data = minimal_serializer::serialize(expected);
		minimal_serializer::deserialize(actual, data);
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE_TEMPLATE(test_member_serialize_size, Test, test_serializable_non_string_types) {
		constexpr auto size = minimal_serializer::serialized_size_v<Test>;
		BOOST_CHECK_EQUAL(get_size<Test>(), size);
	}

	BOOST_AUTO_TEST_CASE(test_serialize_deserialize_offset) {
		constexpr uint64_t expected = 123456789;
		std::vector<uint8_t> buffer(20);
		serialize(expected, buffer, 12);
		uint64_t actual = 0;
		deserialize(actual, buffer, 12);
		BOOST_CHECK_EQUAL(expected, actual);
	}

	BOOST_AUTO_TEST_CASE(test_serialize_offset_out_of_range) {
		constexpr uint64_t value = 0;
		std::vector<uint8_t> buffer(20);
		BOOST_CHECK_THROW(minimal_serializer::serialize(value, buffer, 13), minimal_serializer::serialization_error);
	}

	BOOST_AUTO_TEST_CASE(test_deserialize_offset_out_of_range) {
		uint64_t value = 0;
		const std::vector<uint8_t> buffer(20);
		BOOST_CHECK_THROW(minimal_serializer::deserialize(value, buffer, 13), minimal_serializer::serialization_error);
	}

	BOOST_AUTO_TEST_CASE(test_serialize_deserialize_stream) {
		constexpr uint64_t expected = 123456789;
		std::ostringstream ostream(std::ios::binary);
		serialize(expected, ostream);

		uint64_t actual = 0;
		std::istringstream istream(std::string(ostream.str().data(), ostream.str().size()), std::ios::binary);
		deserialize(actual, istream);

		BOOST_CHECK_EQUAL(expected, actual);
	}

	// Tests for std::array
	BOOST_AUTO_TEST_CASE(test_member_serialize_not_change_array) {
		const std::array<int32_t, 4> expected = {-123, 23, 56, 7};
		const auto actual = expected;
		serialize(actual);
		BOOST_TEST(expected == actual, boost::test_tools::per_element());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_consistency_array) {
		const std::array<int32_t, 4> expected = {-123, 23, 56, 7};
		auto data1 = serialize(expected);
		auto data2 = serialize(expected);
		BOOST_CHECK_EQUAL_COLLECTIONS(data1.begin(), data1.end(), data2.begin(), data2.end());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_deserialize_array) {
		const std::array<int32_t, 4> expected = {-123, 23, 56, 7};
		std::array<int32_t, 4> actual{};
		const auto data = serialize(expected);
		deserialize(actual, data);
		BOOST_TEST(expected == actual, boost::test_tools::per_element());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_size_array) {
		constexpr auto size = minimal_serializer::serialized_size_v<std::array<int32_t, 4>>;
		BOOST_CHECK_EQUAL(sizeof(int32_t)*4, size);
	}

	// Tests for std::tuple
	BOOST_AUTO_TEST_CASE(test_member_serialize_not_change_tuple) {
		const std::tuple<int32_t, uint16_t, bool, fixed_string_t<8>> expected = {
			-123, 23, true, u8"ういw"
		};
		const auto actual = expected;
		serialize(actual);
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_consistency_tuple) {
		const std::tuple<int32_t, uint16_t, bool, fixed_string_t<8>> expected = {
			-123, 23, true, u8"はい?"
		};
		auto data1 = serialize(expected);
		auto data2 = serialize(expected);
		BOOST_CHECK_EQUAL_COLLECTIONS(data1.begin(), data1.end(), data2.begin(), data2.end());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_deserialize_tuple) {
		const std::tuple<int32_t, uint16_t, bool, fixed_string_t<8>> expected = {
			-123, 23, true, u8"はあ!"
		};
		std::tuple<int32_t, uint16_t, bool, fixed_string_t<8>> actual{};
		const auto data = serialize(expected);
		deserialize(actual, data);
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_size_tuple) {
		constexpr auto size = minimal_serializer::serialized_size_v<std::tuple<
			int32_t, uint16_t, bool, fixed_string_t<8>>>;
		BOOST_CHECK_EQUAL(sizeof(int32_t) + sizeof(uint16_t) + 1 + 8, size);
	}

	// Tests for fixed_string or fixed_u8string
	BOOST_AUTO_TEST_CASE(test_member_serialize_not_change_fixed_string) {
		const fixed_string_t<16> expected = u8"おはよu";
		const auto actual = expected;
		serialize(actual);
		BOOST_CHECK_EQUAL(expected, actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_consistency_fixed_string) {
		const fixed_string_t<32> expected = u8"さよなら123";
		auto data1 = serialize(expected);
		auto data2 = serialize(expected);
		BOOST_CHECK_EQUAL_COLLECTIONS(data1.begin(), data1.end(), data2.begin(), data2.end());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_deserialize_fixed_string) {
		const fixed_string_t<16> expected = u8"こんbanわ";
		fixed_string_t<16> actual{};
		const auto data = serialize(expected);
		deserialize(actual, data);
		BOOST_CHECK_EQUAL(expected, actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_size_fixed_string) {
		constexpr auto size = minimal_serializer::serialized_size_v<fixed_string_t<16>>;
		BOOST_CHECK_EQUAL(16, size);
	}

	// Tests for boost::static_string or boost:static_u8string
	BOOST_AUTO_TEST_CASE(test_member_serialize_not_change_boost_static_string) {
		const boost_static_string_t<16> expected = u8"おはよu";
		const auto actual = expected;
		serialize(actual);
		// Use BOOST_CHECK instead of BOOST_CHECK_EQUAL because boost::static_u8string does not support ostream.
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_consistency_boost_static_string) {
		const boost_static_string_t<32> expected = u8"さよなら123";
		auto data1 = serialize(expected);
		auto data2 = serialize(expected);
		BOOST_CHECK_EQUAL_COLLECTIONS(data1.begin(), data1.end(), data2.begin(), data2.end());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_deserialize_boost_static_string) {
		const boost_static_string_t<16> expected = u8"こんbanわ";
		boost_static_string_t<16> actual{};
		const auto data = serialize(expected);
		deserialize(actual, data);
		// Use BOOST_CHECK instead of BOOST_CHECK_EQUAL because boost::static_u8string does not support ostream.
		BOOST_CHECK(expected == actual);
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_size_boost_static_string) {
		constexpr auto size = minimal_serializer::serialized_size_v<boost_static_string_t<16>>;
		BOOST_CHECK_EQUAL(16, size);
	}

	// Tests of portability between boost::static_string / boost:static_u8string and fixed_string / fixed_u8string
	BOOST_AUTO_TEST_CASE(test_member_serialize_deserialize_portability_boost_static_string_to_fixed_string) {
		const boost_static_string_t<16> expected = u8"こんbanわ";
		fixed_string_t<16> actual{};
		const auto data = serialize(expected);
		deserialize(actual, data);
		// Use BOOST_CHECK instead of BOOST_CHECK_EQUAL because std::u8string does not support ostream.
		BOOST_CHECK(std_string_t(expected.c_str()) == actual.to_string());
	}

	BOOST_AUTO_TEST_CASE(test_member_serialize_deserialize_portability_fixed_string_to_boost_static_string) {
		const fixed_string_t<16> expected = u8"こんbanわ";
		boost_static_string_t<16> actual{};
		const auto data = serialize(expected);
		deserialize(actual, data);
		// Use BOOST_CHECK instead of BOOST_CHECK_EQUAL because std::u8string does not support ostream.
		BOOST_CHECK(expected.to_string() == std_string_t(actual.c_str()));
	}

BOOST_AUTO_TEST_SUITE_END()
