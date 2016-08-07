/* ScummVM - Graphic Adventure Engine
*
* ScummVM is the legal property of its developers, whose names
* are too numerous to list here. Please refer to the COPYRIGHT
* file distributed with this source distribution.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#include "macventure/macventure.h"
#include "macventure/tests/tests.h"

namespace MacVenture {
namespace Tests {

DEF_TEST(dummy, ;)

DEF_TEST(adding_works,
	ASSERT_TRUE(1 + 1 == 2);
)

DEF_TEST(assign_possible,
	int number = 1;
	ASSERT_TRUE(number == 1);
)

DEF_TEST(multiple_assert_tests,
	int i = 0;
	ASSERT_TRUE(++i == 1);
	ASSERT_TRUE(++i == 2);
	ASSERT_TRUE(++i == 3);
)

DEF_TEST(failing_test,
	ASSERT_TRUE(1 + 1 == 4);
)

DEF_TEST(test_assert_false,
	ASSERT_FALSE(1 == 2);
)

DEF_TEST(test_assert_eq,
	ASSERT_EQUALS((1 + 1), 2);
)


DEF_TEST(mock_container,
	Container *scriptContainer = new Container("Shadowgate II/Shadow Filter");
	ASSERT_TRUE(scriptContainer->getItemByteSize(1) != 0);
)

static const TestCase tests[] = {
	// Framework tests
	TEST_CASE("Dummy", dummy),
	TEST_CASE("One Plus one is two", adding_works),
	TEST_CASE("Test framework allows assignments", assign_possible),
	TEST_CASE("Multiple asserts allowed", multiple_assert_tests),
	TEST_CASE("This is supposed to fail", failing_test),
	TEST_CASE("One is different than two", test_assert_false),
	TEST_CASE("One plus one is still two", test_assert_eq),
	// Engine tests
	TEST_CASE("Containers can be instantiated", *mock_container),
	TEST_CASE_END_SUITE()
};

void runTests() {
	debug("[ RUN TESTS ]");
	int i = 0;
	while (tests[i].name != "END_SUITE") {
		bool testResult = tests[i].testFunction();
		Common::String resultMessage = testResult ? " OK " : "FAIL";
		debug("[ %s ] %s", resultMessage.c_str(), tests[i].name.c_str());
		i++;
	}
}

} // End of namespace MacVenture::Tests
} // End of namespace MacVenture
