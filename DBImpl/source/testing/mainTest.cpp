/*
 * mainTest.cpp
 *
 *	Purpose: Launches Unit and Integration Tests
 *
 *  @author David Grotzky
 *  @author Roman Schwienbacher
 */

#include "mainTest.h"
#include "gen.cpp"
#include "../gtest/gtest.h"
#include <iostream>
#include "../sorting/sorter.h"
#include "buffertest.cpp"
#include <math.h>
#include <iostream>

using namespace std;
using namespace sorting;

namespace testing {

int g_argc;
char** g_argv;

TEST (ExternalSortTest, CompleteRun) {

	// check input parameter

	unsigned n = atoi(g_argv[3]);
	if (n == 0) {
		cerr << "invalid length: " << g_argv[3] << std::endl;
		return;
	}

	// 671088640 * 8 Byte = 5 GB
	//const uint inputSize = 671088640;

	// 262144 * 8 Byte = 2 MB
	const uint inputSize = 262144;

	// memory size in MB
	const uint memorySize = n * 1024 * 1024;

	// generate input file
	RandomLong::generateInputFile(g_argv[1], inputSize);

	cout << "testfile generated, now sort content" << endl;

	// open file poiners
	int fdInput = open(g_argv[1], O_RDONLY);
	int fdOutput = open(g_argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

	ASSERT_GE(fdInput, 0);
	ASSERT_GE(fdOutput, 0);

	// call sorting
	Sorter sorter;
	sorter.externalSort(fdInput, inputSize, fdOutput, memorySize);

	// reopen output pointer
	fdOutput = open("/tmp/output.txt", O_RDONLY);

	ASSERT_GE(fdOutput, 0);

	cout << "content sorted, now check content" << endl;

	// calculate needed chunks
	int chunks = (int) ceil((double) (inputSize * 8) / memorySize);
	uint valsPerChunk = memorySize / 8;
	uint64_t valsProcessed = 0;
	uint64_t oldVal = 0;
	uint64_t newVal = 0;

	// iterate over chunks
	for (int i = 0; i < chunks; ++i) {

		// char-buffer pointer with size of given memSize
		char* charBufferPointer = new char[memorySize];

		// read file content into buffer
		ASSERT_GE(read(fdOutput, charBufferPointer, memorySize), 0);

		// cast char buffer into uint64_t buffer
		uint64_t* uintBufferPointer = (uint64_t*) charBufferPointer;

		// iterate over vals and test
		for (uint j = 0; j < valsPerChunk && valsProcessed < inputSize; ++j) {

			newVal = *uintBufferPointer++;

			// ignore end-of-file
			if (newVal == 0) {
				continue;
			}

			// newVal must be >= than oldVal
			ASSERT_GE(newVal, oldVal);

			oldVal = newVal;

			++valsProcessed;
		}
	}

	ASSERT_GE(close(fdOutput), 0);
}

TEST (BufferTest, CompleteRun) {
	ASSERT_EQ(testing::launchBuffertest(g_argv), 0);
}

}

/*
 * launch all tests
 */
int main(int argc, char **argv) {

	// check input parameter
	if (argc < 8) {
		cerr << "usage of whole test: " << argv[0] << " <sort_inputFile> <sort_outputFile> <sort_memoryBufferInMB> <buf_file> <buf_pagesOnDisk> <buf_pagesInRAM> <buf_threads>" << endl;
		exit(0);
	}

	::testing::InitGoogleTest(&argc, argv);

	// assign global attributes to use in test method
	testing::g_argc = argc;
	testing::g_argv = argv;

	return RUN_ALL_TESTS();
}

