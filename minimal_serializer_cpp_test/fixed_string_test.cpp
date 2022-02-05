/*
The MIT License (MIT)

Copyright (c) 2019 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <boost/test/unit_test.hpp>
#include <boost/config.hpp>

#include "minimal_serializer/fixed_string.hpp"

#if BOOST_CXX_VERSION < 202002L
using string_t = std::string;
template<std::size_t Length>
using fixed_string_t = minimal_serializer::fixed_string<Length>;
#else
using string_t = std::u8string;
template<std::size_t Length>
using fixed_string_t = minimal_serializer::fixed_u8string<Length>;
#endif

std::ostream& operator <<(std::ostream& os, const std::u8string& str) {
	os << reinterpret_cast<const char*>(str.c_str());
	return os;
}

BOOST_AUTO_TEST_SUITE(fixed_string_test)
	BOOST_AUTO_TEST_CASE(test_c_str_construct_alphabet) {
		const auto test = u8"ABCXYZ";
		BOOST_CHECK(string_t(test) == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_c_str_construct_japanese) {
		const auto test = u8"あいうえお蟹";
		BOOST_CHECK(string_t(test) == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_c_str_construct_mixed) {
		const auto test = u8"あいうABCえお蟹";
		BOOST_CHECK(string_t(test) == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_std_string_construct_alphabet) {
		const string_t test = u8"ABCXYZ";
		BOOST_CHECK(test == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_std_string_construct_japanese) {
		const string_t test = u8"あいうえお蟹";
		BOOST_CHECK(test == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_std_string_construct_mixed) {
		const string_t test = u8"あいうABCえお蟹";
		BOOST_CHECK(test == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_construct_empty) {
		const string_t test{};
		BOOST_CHECK(test == fixed_string_t<32>(test).to_string());
	}

	BOOST_AUTO_TEST_CASE(test_construct_bound_length_string) {
		fixed_string_t<15>(u8"あああああ");
	}

	BOOST_AUTO_TEST_CASE(test_construct_too_long_string) {
		BOOST_CHECK_EXCEPTION(fixed_string_t<32>(u8"あああああああああああ"), std::out_of_range,
							[](auto) {return true; });
	}

	BOOST_AUTO_TEST_CASE(test_copy_construct) {
		const string_t test = u8"かきくけこ";
		const fixed_string_t<32> original(test);
		const auto copied(original);
		BOOST_CHECK(test == original.to_string());
		BOOST_CHECK(test == copied.to_string());
	}

	BOOST_AUTO_TEST_CASE(test_copy_operator) {
		const string_t test = u8"かきくけこ";
		const fixed_string_t<32> original(test);
		fixed_string_t<32> copied;
		copied = original;
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


BOOST_AUTO_TEST_SUITE_END()
