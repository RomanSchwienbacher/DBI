/*
 * operatorTest.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: DRomanAvid
 */

#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "../operators/register.h"
#include "../../parsinglib/Types.hpp"

using namespace std;

namespace testing {

static void operatorTest() {

	cout << "Creating Registers" << endl;

	// test Integer <
	Register intReg1(1);
	Register intReg2(2);
	assert(intReg1 < intReg2);
	assert(!(intReg2 < intReg1));

	// test Character <, also test prefix chars
	Char<2> char1, char2;
	char1.loadString("DE");
	char2.loadString("US");
	Char<3> char3;
	char3.loadString("USA");

	Register charReg1(char1);
	Register charReg2(char2);
	Register charReg3(char3);
	assert(charReg1 < charReg2);
	assert(charReg1 < charReg3);
	assert(!(charReg2 < charReg1));
	assert(!(charReg3 < charReg1));


	// test setChar
	char2.loadString("AU");
	charReg2.setChar(char2);
	assert(charReg2 < charReg1);
	assert(intReg1 < charReg1);
	assert(!(charReg1 < intReg1));

	// test Char ==
	char2.loadString("DE");
	charReg2.setChar(char2);
	// invalid setting has no effect
	charReg2.setInteger(555);
	assert(charReg1 == charReg2);

	// test setInteger
	intReg2.setInteger(1);
	// invalid setting has no effect
	intReg2.setChar(char2);

	// test Integer ==
	assert(intReg1 == intReg2);

}

static int launchOperatorTest(char** argv) {
	operatorTest();
	return 0;
}

}

