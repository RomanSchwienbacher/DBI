/*
 * mainTest.cpp
 *
 *	Purpose: Launches Unit and Integration Tests
 *
 *  @author DRomanAvid
 */

#include "mainTest.h"
#include "gen.cpp"
#include "../../testinglib/gtest/gtest.h"
#include <iostream>
#include "../sorting/sorter.h"
#include "buffertest.cpp"
#include "slottedtest.cpp"
#include "bTreeTest.cpp"
#include "schemaTest.cpp"
#include "operatorTest.cpp"
#include <math.h>

using namespace std;
using namespace sorting;

namespace testing {

// Defines which test types can be invoked
enum class TestType {
	EXTERNALSORT_TEST, BUFFER_TEST, SLOTTED_TEST, BTREE_TEST, SCHEMA_TEST, OPERATOR_TEST
};

char** g_argv;
TestType g_type;

TEST (ExternalSortTest, CompleteRun) {

	if (testing::g_type == testing::TestType::EXTERNALSORT_TEST) {

		cout << "***LAUNCH EXTERNAL-SORT TEST***" << endl;

		// check input parameters

		unsigned n = atoi(g_argv[4]);
		if (n == 0) {
			cerr << "invalid length: " << g_argv[4] << std::endl;
			return;
		}

		// 671088640 * 8 Byte = 5 GB
		//const uint inputSize = 671088640;

		// 262144 * 8 Byte = 2 MB
		const uint inputSize = 262144;

		// memory size in MB
		const uint memorySize = n * 1024 * 1024;

		// generate input file
		RandomLong::generateInputFile(g_argv[2], inputSize);

		cout << "testfile generated, now sort content" << endl;

		// open file poiners
		int fdInput = open(g_argv[2], O_RDONLY);
		int fdOutput = open(g_argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

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
}

TEST (BufferTest, CompleteRun) {
	if (testing::g_type == testing::TestType::BUFFER_TEST) {
		cout << "***LAUNCH BUFFER TEST***" << endl;
		ASSERT_EQ(testing::launchBuffertest(g_argv), 0);
	}
}

TEST (SlottedTest, CompleteRun) {
	if (testing::g_type == testing::TestType::SLOTTED_TEST) {
		cout << "***LAUNCH SLOTTED TEST***" << endl;
		ASSERT_EQ(testing::launchSlottedtest(g_argv), 0);
	}
}

TEST (BTreeTest, CompleteRun) {
	if (testing::g_type == testing::TestType::BTREE_TEST) {
		cout << "***LAUNCH B+TREE TEST***" << endl;
		ASSERT_EQ(testing::launchBTreetest(g_argv), 0);
	}
}

TEST (SchemaTest, CompleteRun) {
	if (testing::g_type == testing::TestType::SCHEMA_TEST) {
		cout << "***LAUNCH SCHEMA TEST***" << endl;
		ASSERT_EQ(testing::launchSchematest(g_argv), 0);
	}
}

TEST (OperatorTest, CompleteRun) {
	if (testing::g_type == testing::TestType::OPERATOR_TEST) {
		cout << "***LAUNCH OPERATOR TEST***" << endl;
		ASSERT_EQ(testing::launchOperatorTest(g_argv), 0);
	}
}

}

/*
 * launches test
 */
int main(int argc, char **argv) {

	// check input param
	if (argc < 2) {
		cerr << "test type not given: [0=ExternalSort-Test, 1=Buffer-Test, 2=Slotted-Test, 3=BTree-Test, 4=Schema-Test] " << argv[0] << endl;
		exit(0);
	}

	unsigned type = atoi(argv[1]);

	/*
	 * external sort test
	 *
	 * possible args:
	 * 0
	 * /tmp/input.txt
	 * /tmp/output.txt
	 * 1
	 */
	if (type == 0) {

		testing::g_type = testing::TestType::EXTERNALSORT_TEST;

		if (argc < 5) {
			cerr << "usage of external sort test: " << argv[0] << " <test_type> <sort_inputFile> <sort_outputFile> <sort_memoryBufferInMB>" << endl;
			exit(0);
		}
	}
	/*
	 * buffer test
	 *
	 * possible args:
	 * 1
	 * /tmp/db.txt
	 * 512
	 * 128
	 * 0
	 */
	else if (type == 1) {

		testing::g_type = testing::TestType::BUFFER_TEST;

		if (argc < 6) {
			cerr << "usage of buffer test: " << argv[0] << " <test_type> <buf_file> <buf_pagesOnDisk> <buf_pagesInRAM> <buf_threads>" << endl;
			exit(0);
		}
	}
	/*
	 * slotted test
	 *
	 * possible args:
	 * 2
	 * /tmp/db.txt
	 * 20
	 */
	else if (type == 2) {

		testing::g_type = testing::TestType::SLOTTED_TEST;

		if (argc < 4) {
			cerr << "usage of slotted test: " << argv[0] << " <test_type> <buf_file> <slot_pageSize>" << endl;
			exit(0);
		}
	}
	/*
	 * b-tree test
	 *
	 * possible args:
	 * 3
	 * /tmp/db.txt
	 * 1000
	 */
	else if (type == 3) {

		testing::g_type = testing::TestType::BTREE_TEST;

		if (argc < 4) {
			cerr << "usage of b-tree test: " << argv[0] << " <test_type> <buf_file> <btree_items>" << endl;
			exit(0);
		}
	}
	/*
	 * schema test
	 *
	 * possible args:
	 * 4
	 * /tmp/db.txt
	 * /tmp/schema.sql
	 */
	else if (type == 4) {

		testing::g_type = testing::TestType::SCHEMA_TEST;

		if (argc < 4) {
			cerr << "usage of schema test: " << argv[0] << " <test_type> <buf_file> <schema_file>" << endl;
			exit(0);
		}
	}

	else if (type == 5) {

		testing::g_type = testing::TestType::OPERATOR_TEST;

	}

	::testing::InitGoogleTest(&argc, argv);

	// assign global attributes to use in test method
	testing::g_argv = argv;

	return RUN_ALL_TESTS();
}
