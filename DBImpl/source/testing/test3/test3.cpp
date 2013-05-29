/*
 * test3.cpp
 *
 *	Purpose: Launches Unit and Integration Tests
 *
 *  @author DRomanAvid
 */

#include "../gen.cpp"
#include "../../../testinglib/gtest/gtest.h"
#include <iostream>
#include "../../sorting/sorter.h"
#include "segTest.cpp"
#include <math.h>
#include <iostream>

using namespace std;
using namespace sorting;

namespace testing {

int g_argc;
char** g_argv;

TEST (SlottedTest, CompleteRun) {
	ASSERT_EQ(testing::launchSlottedtest(g_argv), 0);
}

}

int main(int argc, char **argv) {

	// check input parameter
	if (argc < 3) {
		cerr << "usage of segment test: " << argv[0] << " <inputFile> <pageSize> " << endl;
		exit(0);
	}

	::testing::InitGoogleTest(&argc, argv);

	// assign global attributes to use in test method
	testing::g_argc = argc;
	testing::g_argv = argv;

	return RUN_ALL_TESTS();
}
