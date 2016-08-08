#include "ktest.h"

namespace KWorks {
namespace Tests {

void runTests() {
	print("[ RUN TESTS ]");
	int i = 0;
	while (tests[i].name[0] != '\0') {
		bool testResult = tests[i].testFunction();
		if (testResult) {
			print("[%c[1;32m  OK  %c[0m] %s", 27, 27, tests[i].name);
		} else {
			print("[%c[1;31m FAIL %c[0m] %s", 27, 27, tests[i].name);
		}
		i++;
	}
}

} // End of namespace KWorks::Tests
} // End of namespace KWorks
