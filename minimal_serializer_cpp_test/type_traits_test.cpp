/*
The MIT License (MIT)

Copyright (c) 2019-2022 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <array>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "minimal_serializer/type_traits.hpp"

#include "serialize_test_types.hpp"

using namespace std;
using namespace minimal_serializer;

struct custom_struct final {};

using tuple_like_types = boost::mpl::list<std::array<uint32_t, 24>, std::tuple<bool, int8_t, uint64_t>, std::pair<
											int8_t, bool>>;

using not_tuple_like_types = boost::mpl::list<int8_t, uint8_t, int16_t, uint16_t,
											int32_t, uint32_t, int64_t, uint64_t, bool, custom_struct>;

BOOST_AUTO_TEST_SUITE(type_traits_test)
	BOOST_AUTO_TEST_CASE_TEMPLATE(test_is_tuple_like_true, Test, tuple_like_types) {
		BOOST_CHECK_EQUAL(true, is_tuple_like_v<Test>);
	}

	BOOST_AUTO_TEST_CASE_TEMPLATE(test_is_tuple_like_false, Test, not_tuple_like_types) {
		BOOST_CHECK_EQUAL(false, is_tuple_like_v<Test>);
	}

	BOOST_AUTO_TEST_CASE_TEMPLATE(is_serializable_v_true, Test, test_serializable_types) {
		BOOST_CHECK_EQUAL(true, is_serializable_v<Test>);
	}

#if BOOST_CXX_VERSION >= 202002L
	BOOST_AUTO_TEST_CASE_TEMPLATE(serializable_concept_true, Test, test_serializable_types) {
		BOOST_CHECK_EQUAL(true, serializable<Test>);
	}
#endif

BOOST_AUTO_TEST_SUITE_END()
