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

struct Containers {
	MacVenture::Container *scriptContainer;
	MacVenture::Container *graphicContainer;
	MacVenture::Container *soundContainer;
	MacVenture::Container *textContainer;
	MacVenture::Container *objectContainer;
};

DEF_MOCK_SETUP(mock_containers, Containers,
	target.scriptContainer = new MacVenture::Container("Shadowgate II/Shadow Filter");
	target.graphicContainer = new MacVenture::Container("Shadowgate II/Shadow Graphic");
	target.soundContainer = new MacVenture::Container("Shadowgate II/Shadow Sound");
	target.textContainer = new MacVenture::Container("Shadowgate II/Shadow Text");
	target.objectContainer = new MacVenture::Container("Shadowgate II/Shadow Object");
)

DEF_MOCK_TEARDOWN(mock_containers, Containers,
	delete target.scriptContainer;
	delete target.graphicContainer;
	delete target.soundContainer;
	delete target.textContainer;
	delete target.objectContainer;
)

DEF_TEST(mock_stack_image,
	Containers containers;
	MOCK_SETUP(mock_containers, containers);
	MacVenture::ImageAsset imageOnTheStack(538, containers.graphicContainer);
	ASSERT_TRUE(imageOnTheStack.getWidth() > 0);
	ASSERT_TRUE(imageOnTheStack.getHeight() > 0);
	MOCK_TEARDOWN(mock_containers, containers);
)

DEF_TEST(mock_heap_image,
	Containers containers;
	MOCK_SETUP(mock_containers, containers);
	MacVenture::ImageAsset *imageOnTheHeap = new MacVenture::ImageAsset(538, containers.graphicContainer);
	ASSERT_TRUE(imageOnTheHeap->getWidth() > 0);
	ASSERT_TRUE(imageOnTheHeap->getHeight() > 0);
	MOCK_TEARDOWN(mock_containers, containers);
	delete imageOnTheHeap;
)

DEF_TEST(blit_image_simple,
	Containers containers;
	MOCK_SETUP(mock_containers, containers);
	Graphics::ManagedSurface srf;
	srf.create(MacVenture::kScreenWidth, MacVenture::kScreenHeight, Graphics::PixelFormat::createFormatCLUT8());
	MacVenture::ImageAsset smallImage(538, containers.graphicContainer);
	smallImage.blitInto(&srf, 0, 0, MacVenture::kBlitBIC);
	ASSERT_TRUE(true);
	MOCK_TEARDOWN(mock_containers, containers);
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
	// Valngrind tests
	TEST_CASE("Images on the stack don't leak", mock_stack_image),
	TEST_CASE("Images on the heap don't leak", mock_heap_image),
	TEST_CASE("Blitting images onto a surface doesn't leak", blit_image_simple),
	TEST_CASE_END_SUITE()
};


} // End of namespace KWorks::Tests
} // End of namespace KWorks
