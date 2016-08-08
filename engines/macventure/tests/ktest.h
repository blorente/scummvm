#ifndef KWORKS_TESTS_H
#define KWORKS_TESTS_H

namespace KWorks {
namespace Tests {
// Testing tools
#define DEF_TEST(funcname, code) bool funcname() { code; return true; }
#define ASSERT_TRUE(x) if (!(x)) return false;
#define ASSERT_FALSE(x) if (x) return false;
#define ASSERT_EQUALS(x, y) if ((x) != (y)) return false;

#define DEF_MOCK_SETUP(name, type, code) void name##_setup(type &target) { code; }
#define DEF_MOCK_TEARDOWN(name, type, code) void name##_teardown(type &target) { code; }
#define MOCK_SETUP(name, target) name##_setup(target);
#define MOCK_TEARDOWN(name, target) name##_teardown(target);

#define TEST_CASE(name, fptr)  { name, fptr }
#define TEST_CASE_END_SUITE() { "\0", nullptr }

struct TestCase {
	const char *name;
	bool (*testFunction)(void);
};

extern TestCase tests[];

extern void runTests();

// These have to be implemented in the file using it
extern void print(const char *msg);
extern void print(const char *msg, int a, int b, const char *extra);

} // End of namespace KWorks::Tests
} // End of namespace KWorks

#endif
