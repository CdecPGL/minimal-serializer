/*
The MIT License (MIT)

Copyright (c) 2019 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <boost/test/unit_test.hpp>

#include "minimal_serializer/string_utility.hpp"

BOOST_AUTO_TEST_SUITE(string_utilities_test)
	BOOST_AUTO_TEST_CASE(test_string_concatenation) {
		BOOST_CHECK_EQUAL("ABCXYZあいう", minimal_serializer::generate_string(u8"ABC", u8"XYZ", u8"あいう"));
	}

	BOOST_AUTO_TEST_CASE(test_integer_concatenation) {
		BOOST_CHECK_EQUAL("123-456", minimal_serializer::generate_string(123, -456));
	}

	BOOST_AUTO_TEST_CASE(test_char_concatenation) {
		BOOST_CHECK_EQUAL("1230", minimal_serializer::generate_string(static_cast<char>(123), static_cast<char>(0)));
	}

	BOOST_AUTO_TEST_CASE(test_float_concatenation) {
		BOOST_CHECK_EQUAL("0.12-0.34", minimal_serializer::generate_string(0.12f, -0.34f));
	}

	BOOST_AUTO_TEST_CASE(test_double_concatenation) {
		// double is rounded by 6 digits
		BOOST_CHECK_EQUAL("0.123457-0.987654", minimal_serializer::generate_string(0.123456789, -0.987654321));
	}

	BOOST_AUTO_TEST_CASE(test_bool_concatenation) {
		BOOST_CHECK_EQUAL("truefalse", minimal_serializer::generate_string(true, false));
	}

	BOOST_AUTO_TEST_CASE(test_typeinfo_concatenation) {
		BOOST_CHECK_EQUAL(std::string(typeid(int).name()) + typeid(bool).name(),
						minimal_serializer::generate_string(typeid(int), typeid(bool)));
	}

	enum class test_enum { element1, element2 };

	BOOST_AUTO_TEST_CASE(test_enum_concatenation) {
		if constexpr (minimal_serializer::is_generate_enum_string_supported) {
			BOOST_CHECK_EQUAL("element1element2",
							minimal_serializer::generate_string(test_enum::element1, test_enum::element2));
		}
		else {
			BOOST_TEST_MESSAGE(
				"Enum type values are not convertied to string because the feature is not supported in current compiler.");
			BOOST_CHECK_EQUAL("01", minimal_serializer::generate_string(test_enum::element1, test_enum::element2));
		}
	}

	BOOST_AUTO_TEST_CASE(test_mixed_concatenation) {
		// double is rounded by 6 digits
		if constexpr (minimal_serializer::is_generate_enum_string_supported) {
			BOOST_CHECK_EQUAL("test1true0.230.888889element1",
							minimal_serializer::generate_string("test", 1, true, 0.23, 0.8888888888, test_enum::element1
							));
		}
		else {
			BOOST_TEST_MESSAGE(
				"Enum type values are not convertied to string because the feature is not supported in current compiler.");
			BOOST_CHECK_EQUAL("test1true0.230.8888890",
							minimal_serializer::generate_string("test", 1, true, 0.23, 0.8888888888, test_enum::element1
							));
		}
	}

	BOOST_AUTO_TEST_CASE(test_empty_parameter) {
		BOOST_CHECK_EQUAL("", minimal_serializer::generate_string());
	}

	BOOST_AUTO_TEST_CASE(test_one_parameter) {
		BOOST_CHECK_EQUAL("1", minimal_serializer::generate_string(1));
	}

	BOOST_AUTO_TEST_CASE(test_many_parameters) {
		BOOST_CHECK_EQUAL("12345678910", minimal_serializer::generate_string(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
	}

BOOST_AUTO_TEST_SUITE_END()
