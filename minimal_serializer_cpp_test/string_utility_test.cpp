/*
The MIT License (MIT)

Copyright (c) 2019-2022 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <boost/test/unit_test.hpp>

#include "minimal_serializer/string_utility.hpp"
#include "minimal_serializer/fixed_string.hpp"

using namespace std;
using namespace minimal_serializer;

BOOST_AUTO_TEST_SUITE(string_utilities_test)
	BOOST_AUTO_TEST_CASE(test_char_array) {
		BOOST_CHECK_EQUAL("ABCXYZ", generate_string("ABCXYZ"));
	}

	BOOST_AUTO_TEST_CASE(test_char_array_pointer) {
		BOOST_CHECK_EQUAL("ABCXYZ", generate_string(&"ABCXYZ"[0]));
	}

	BOOST_AUTO_TEST_CASE(test_string) {
		BOOST_CHECK_EQUAL("ABCXYZ", generate_string("ABCXYZ"s));
	}

	BOOST_AUTO_TEST_CASE(test_fixed_string) {
		BOOST_CHECK_EQUAL("ABCXYZ", generate_string(fixed_string<8>("ABCXYZ")));
	}

	BOOST_AUTO_TEST_CASE(test_u8char_array) {
		BOOST_CHECK_EQUAL("あいうえお", generate_string(u8"あいうえお"));
	}

	BOOST_AUTO_TEST_CASE(test_u8char_array_pointer) {
		BOOST_CHECK_EQUAL("あいうえお", generate_string(&u8"あいうえお"[0]));
	}

	BOOST_AUTO_TEST_CASE(test_u8string) {
		BOOST_CHECK_EQUAL("あいうえお", generate_string(u8"あいうえお"s));
	}

#if BOOST_CXX_VERSION >= 202002L
	BOOST_AUTO_TEST_CASE(test_fixed_u8string) {
		BOOST_CHECK_EQUAL("ABCXYZ", generate_string(fixed_u8string<8>(u8"ABCXYZ")));
	}
#endif

	BOOST_AUTO_TEST_CASE(test_string_concatenation) {
		BOOST_CHECK_EQUAL("ABCあいう", generate_string("ABC", u8"あいう"));
	}

	BOOST_AUTO_TEST_CASE(test_byte_concatenation) {
		BOOST_CHECK_EQUAL("123-123", generate_string(static_cast<uint8_t>(123), static_cast<int8_t>(-123)));
	}

	BOOST_AUTO_TEST_CASE(test_integer_concatenation) {
		BOOST_CHECK_EQUAL("123-456", generate_string(123, -456));
	}

	BOOST_AUTO_TEST_CASE(test_float_concatenation) {
		BOOST_CHECK_EQUAL("0.12-0.34", generate_string(0.12f, -0.34f));
	}

	BOOST_AUTO_TEST_CASE(test_double_concatenation) {
		// double is rounded by 6 digits
		BOOST_CHECK_EQUAL("0.123457-0.987654", generate_string(0.123456789, -0.987654321));
	}

	BOOST_AUTO_TEST_CASE(test_bool_concatenation) {
		BOOST_CHECK_EQUAL("truefalse", generate_string(true, false));
	}

	BOOST_AUTO_TEST_CASE(test_typeinfo_concatenation) {
		BOOST_CHECK_EQUAL(std::string(typeid(int).name()) + typeid(bool).name(),
						generate_string(typeid(int), typeid(bool)));
	}

	enum class test_enum { element1, element2 };

	BOOST_AUTO_TEST_CASE(test_enum_concatenation) {
		if constexpr (is_generate_enum_string_supported) {
			BOOST_CHECK_EQUAL("element1element2",
							generate_string(test_enum::element1, test_enum::element2));
		}
		else {
			BOOST_TEST_MESSAGE(
				"Enum type values are not convertied to string because the feature is not supported in current compiler.");
			BOOST_CHECK_EQUAL("01", generate_string(test_enum::element1, test_enum::element2));
		}
	}

	BOOST_AUTO_TEST_CASE(test_mixed_concatenation) {
		// double is rounded by 6 digits
		if constexpr (is_generate_enum_string_supported) {
			BOOST_CHECK_EQUAL("test1true0.230.888889element1",
							generate_string("test", 1, true, 0.23, 0.8888888888, test_enum::element1
							));
		}
		else {
			BOOST_TEST_MESSAGE(
				"Enum type values are not convertied to string because the feature is not supported in current compiler.");
			BOOST_CHECK_EQUAL("test1true0.230.8888890",
							generate_string("test", 1, true, 0.23, 0.8888888888, test_enum::element1
							));
		}
	}

	BOOST_AUTO_TEST_CASE(test_empty_parameter) {
		BOOST_CHECK_EQUAL("", generate_string());
	}

	BOOST_AUTO_TEST_CASE(test_many_parameters) {
		BOOST_CHECK_EQUAL("12345678910", generate_string(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
	}

BOOST_AUTO_TEST_SUITE_END()
