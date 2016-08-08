#include "macventure/macventure.h"
#include "common/debug.h"
#include "macventure/tests/ktest.h"

namespace KWorks {
namespace Tests {

void print(const char *msg) {
	debug(msg);
}
void print(const char *msg, int a, int b, const char *extra) {
	debug(msg, a, b, extra);
}

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
	MacVenture::Container *scriptContainer = new MacVenture::Container("Shadowgate II/Shadow Filter");
	ASSERT_TRUE(scriptContainer->getItemByteSize(1) != 0);
)

TestCase tests[] = {
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


} // End of namespace KWorks::Tests
} // End of namespace KWorks
