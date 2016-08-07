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

bool t0() {
	return true;
}

static const TestCase tests[] = {
	TEST_CASE("Dummy", *t0),
	TEST_CASE_END_SUITE()
};

void runTests() {
	debug("[ RUN TESTS ]");
	int i = 0;
	while (tests[i].name != "END_SUITE") {
		bool testResult = tests[i].testFunction();
		Common::String resultMessage = testResult ? "OK" : "FAIL";
		debug("[ TEST ] %s...%s", tests[i].name.c_str(), resultMessage.c_str());
		i++;
	}
}

} // End of namespace MacVenture::Tests
} // End of namespace MacVenture
