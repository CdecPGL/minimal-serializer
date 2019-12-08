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
		BOOST_CHECK_EQUAL(u8"ABCXYZあいう", pgl::generate_string(u8"ABC", u8"XYZ", u8"あいう"));
	}

	BOOST_AUTO_TEST_CASE(test_integer_concatenation) {
		BOOST_CHECK_EQUAL(u8"123-456", pgl::generate_string(123, -456));
	}

	BOOST_AUTO_TEST_CASE(test_float_concatenation) {
		BOOST_CHECK_EQUAL(u8"0.12-0.34", pgl::generate_string(0.12f, -0.34f));
	}

	BOOST_AUTO_TEST_CASE(test_double_concatenation) {
		// double is rounded by 6 digits
		BOOST_CHECK_EQUAL(u8"0.123457-0.987654", pgl::generate_string(0.123456789, -0.987654321));
	}

	BOOST_AUTO_TEST_CASE(test_bool_concatenation) {
		BOOST_CHECK_EQUAL(u8"truefalse", pgl::generate_string(true, false));
	}

	enum class test_enum { element1, element2 };

	BOOST_AUTO_TEST_CASE(test_enum_concatenation) {
		BOOST_CHECK_EQUAL(u8"element1element2", pgl::generate_string(test_enum::element1, test_enum::element2));
	}

	BOOST_AUTO_TEST_CASE(test_mixed_concatenation) {
		// double is rounded by 6 digits
		BOOST_CHECK_EQUAL(u8"test1true0.230.888889element1",
			pgl::generate_string("test", 1, true, 0.23, 0.8888888888, test_enum::element1));
	}

	BOOST_AUTO_TEST_CASE(test_empty_parameter) {
		BOOST_CHECK_EQUAL(u8"", pgl::generate_string());
	}

	BOOST_AUTO_TEST_CASE(test_one_parameter) {
		BOOST_CHECK_EQUAL(u8"1", pgl::generate_string(1));
	}

	BOOST_AUTO_TEST_CASE(test_many_parameters) {
		BOOST_CHECK_EQUAL(u8"12345678910", pgl::generate_string(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
	}

BOOST_AUTO_TEST_SUITE_END()
