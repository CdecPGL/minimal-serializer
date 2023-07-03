/*
The MIT License (MIT)

Copyright (c) 2019 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <unordered_set>

#include <boost/test/unit_test.hpp>

#include "minimal_serializer/fixed_string.hpp"

using namespace std;
using namespace minimal_serializer;

// System encoded strings
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996) // Avoid deprecation of fixed_string and static_string
#endif
template <std::size_t Length>
using fixed_string_se_t = fixed_string<Length>;
using std_string_se_t = std::string;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef __cpp_char8_t
// Standard strings in C++20
template<std::size_t Length>
using fixed_string_t = fixed_u8string<Length>;
using std_string_t = std::u8string;
#else
// Standard strings before C++17
template <std::size_t Length>
using fixed_string_t = fixed_string<Length>;
using std_string_t = std::string;
#endif

BOOST_AUTO_TEST_SUITE(utf8_fixed_string_test)
	BOOST_AUTO_TEST_CASE(test_c_str_construct_alphabet) {
		const auto test = u8"ABCXYZ";
		// Use BOOST_CHECK instead of BOOST_CHECK_EQUAL because BOOST_CHECK does not support u8string due to std::ostream support.
		BOOST_CHECK(std_string_t(test) == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_c_str_construct_japanese) {
		const auto test = u8"あいうえお蟹";
		BOOST_CHECK(std_string_t(test) == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_c_str_construct_mixed) {
		const auto test = u8"あいうABCえお蟹";
		BOOST_CHECK(std_string_t(test) == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_std_string_construct_alphabet) {
		const std_string_t test = u8"ABCXYZ";
		BOOST_CHECK(test == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_std_string_construct_japanese) {
		const std_string_t test = u8"あいうえお蟹";
		BOOST_CHECK(test == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_std_string_construct_mixed) {
		const std_string_t test = u8"あいうABCえお蟹";
		BOOST_CHECK(test == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_construct_empty) {
		const std_string_t test{};
		BOOST_CHECK(test == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_construct_bound_length_string) {
		BOOST_CHECK(std_string_t(u8"あああああ") == fixed_string_t<15>(u8"あああああ").to_string());
	}

	BOOST_AUTO_TEST_CASE(test_construct_too_long_string) {
		BOOST_CHECK_EXCEPTION(fixed_string_t<32>(u8"あああああああああああ"), std::out_of_range,
							[](auto) {return true; });
	}

	BOOST_AUTO_TEST_CASE(test_copy_construct) {
		const std_string_t test = u8"かきくけこ";
		const fixed_string_t<32> original(test);
		const auto copied(original);
		BOOST_CHECK(test == original.to_string());
		BOOST_CHECK(test == copied.to_string());
	}

	BOOST_AUTO_TEST_CASE(test_copy_operator) {
		const std_string_t test = u8"かきくけこ";
		const fixed_string_t<32> original(test);
		const fixed_string_t<32> copied = original;
		BOOST_CHECK(test == original.to_string());
		BOOST_CHECK(test == copied.to_string());
	}

	BOOST_AUTO_TEST_CASE(test_indexer_start) {
		const fixed_string_t<4> test(u8"abcd");
		BOOST_CHECK(test[0] == u8'a');
	}

	BOOST_AUTO_TEST_CASE(test_indexer_middle) {
		const fixed_string_t<4> test(u8"abcd");
		BOOST_CHECK(test[1] == u8'b');
	}

	BOOST_AUTO_TEST_CASE(test_indexer_end) {
		const fixed_string_t<4> test(u8"abcd");
		BOOST_CHECK(test[3] == u8'd');
	}

	BOOST_AUTO_TEST_CASE(test_at_out_of_range) {
		const fixed_string_t<4> test(u8"abcd");
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4834)
#elif __GNUC__
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wunused-result"
#elif __clang__
#pragma CLANG diagnostic push 
#pragma CLANG diagnostic ignored "-Wunused-result"
#endif
		BOOST_CHECK_EXCEPTION(test.at(4), std::out_of_range, [](auto) {return true; });
#ifdef _MSC_VER
#pragma warning(pop)
#elif __GNUC__
#pragma GCC diagnostic pop
#elif __clang__
#pragma CLANG diagnostic pop
#endif
	}

	BOOST_AUTO_TEST_CASE(test_less_than_operator_true) {
		const fixed_string_t<4> small(u8"abcd");
		const fixed_string_t<4> big(u8"abda");
		BOOST_CHECK(small < big);
	}

	BOOST_AUTO_TEST_CASE(test_less_than_operator_false) {
		const fixed_string_t<4> small(u8"abcd");
		const fixed_string_t<4> big(u8"abda");
		BOOST_CHECK(!(big < small));
	}

	BOOST_AUTO_TEST_CASE(test_less_than_operator_same) {
		const fixed_string_t<4> test1(u8"abcd");
		const fixed_string_t<4> test2(u8"abcd");
		BOOST_CHECK(!(test1 < test2));
	}

	BOOST_AUTO_TEST_CASE(test_equal_operator_true) {
		const fixed_string_t<4> test1(u8"abcd");
		const fixed_string_t<4> test2(u8"abcd");
		BOOST_CHECK(test1 == test2);
	}

	BOOST_AUTO_TEST_CASE(test_equal_operator_false) {
		const fixed_string_t<4> test1(u8"abcd");
		const fixed_string_t<4> test2(u8"abdd");
		BOOST_CHECK(!(test1 == test2));
	}

	BOOST_AUTO_TEST_CASE(test_size_zero) {
		const fixed_string_t<32> test1{};
		BOOST_CHECK_EQUAL(0, test1.size());
	}

	BOOST_AUTO_TEST_CASE(test_size_alphabet) {
		const fixed_string_t<32> test1(u8"abcdefg");
		BOOST_CHECK_EQUAL(7, test1.size());
	}

	BOOST_AUTO_TEST_CASE(test_size_alphabet_bound) {
		const fixed_string_t<7> test1(u8"abcdefg");
		BOOST_CHECK_EQUAL(7, test1.size());
	}

	BOOST_AUTO_TEST_CASE(test_size_japanese) {
		const fixed_string_t<32> test1(u8"さしすせそ");
		BOOST_CHECK_EQUAL(15, test1.size());
	}

	BOOST_AUTO_TEST_CASE(test_size_japanese_bound) {
		const fixed_string_t<15> test1(u8"さしすせそ");
		BOOST_CHECK_EQUAL(15, test1.size());
	}

	// boost::output_test_stream is not available due to https://stackoverflow.com/questions/3185380/boost-test-output-test-stream-fails-with-templated-output-operator
	BOOST_AUTO_TEST_CASE(test_ostream) {
		ostringstream ostream;
#ifdef __cpp_char8_t
		ostream << fixed_string_t<32>(u8"こんちは");
#else
		ostream << fixed_string_t<32>("こんちは");
#endif
	BOOST_CHECK_EQUAL("こんちは"s, ostream.str());
	}

	BOOST_AUTO_TEST_CASE(test_std_hash) {
		using fs = fixed_string_t<4>;
		const fs key(u8"あ");
		unordered_set<fs> set;
		set.insert(key);
		BOOST_CHECK(set.find(key) != set.end());
		BOOST_CHECK(set.find(fs(u8"い")) == set.end());
	}

	BOOST_AUTO_TEST_CASE(test_boost_hash) {
		using fs = fixed_string_t<4>;
		const fs key(u8"あ");
		unordered_set<fs, boost::hash<fs>> set;
		set.insert(key);
		BOOST_CHECK(set.find(key) != set.end());
		BOOST_CHECK(set.find(fs(u8"い")) == set.end());
	}


BOOST_AUTO_TEST_SUITE_END()

// Test only ostream operator because other operators are same as fixed_string_t
BOOST_AUTO_TEST_SUITE(system_encoded_fixed_string_test)
	// boost::output_test_stream is not available due to https://stackoverflow.com/questions/3185380/boost-test-output-test-stream-fails-with-templated-output-operator
	BOOST_AUTO_TEST_CASE(test_ostream) {
		ostringstream ostream;
		ostream << fixed_string_se_t<32>("こんちは");
		BOOST_CHECK_EQUAL("こんちは"s, ostream.str());
	}

BOOST_AUTO_TEST_SUITE_END()
