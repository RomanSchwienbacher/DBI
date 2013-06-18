#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "../buffer/bufman.h"
#include "../segments/segman.h"
#include "../segments/sch_segment.h"

using namespace std;

namespace testing {


static void schemaTest(const std::string& dbFilename, const std::string& schemaFilename) {

	/*
	// Set up stuff
	BufferManager bm(dbFilename, 25600ul); // bogus arguments -> 100 MB
	SegmentManager sm(1, 1, &bm);
	uint64_t spId = sm.createSegment(SegmentType::SCHEMA, 10);
	BTreeSegment& seg = (BTreeSegment&) (sm.getSegment(spId));
	BTree<T, CMP> bTree(seg);

	cout << "Insert " << n << " values" << endl;
	for (uint64_t i = 0; i < n; ++i) {

		TID tid;
		tid.pageId = i * i;
		tid.slotId = 0;

		bTree.insert(getKey<T>(i), tid);
	}

	uint64_t bTreeSize = bTree.size();

	if (bTreeSize != n) {
		cerr << "damn, bTreeSize " << bTreeSize << " must be == " << n << endl;
	}
	assert(bTreeSize == n);

	cout << "Check if they can be retrieved" << endl;
	for (uint64_t i = 0; i < n; ++i) {
		TID tid;
		if (!bTree.lookup(getKey<T>(i), tid)) {
			cerr << "damn, tid was not found by key" << endl;
		}
		assert(bTree.lookup(getKey<T>(i), tid));

		if (tid.pageId != i * i) {
			cerr << "damn, pageId " << tid.pageId << " must be == " << (i * i) << endl;
		}
		assert(tid.pageId == i * i);
	}

	cout << "Delete some values" << endl;
	for (uint64_t i = 0; i < n; ++i) {
		if ((i % 7) == 0) {
			bTree.erase(getKey<T>(i));
		}
	}

	cout << "Check if the right ones have been deleted" << endl;
	for (uint64_t i = 0; i < n; ++i) {
		TID tid;
		if ((i % 7) == 0) {
			if (bTree.lookup(getKey<T>(i), tid)) {
				cerr << "damn, tid was found by key (should be deleted)" << endl;
			}
			assert(!bTree.lookup(getKey<T>(i), tid));
		} else {
			if (!bTree.lookup(getKey<T>(i), tid)) {
				cerr << "damn, tid was not found by key" << endl;
			}
			assert(bTree.lookup(getKey<T>(i), tid));

			if (tid.pageId != i * i) {
				cerr << "damn, pageId " << tid.pageId << " must be == " << (i * i) << endl;
			}
			assert(tid.pageId == i * i);
		}
	}

	cout << "Delete everything" << endl;
	for (uint64_t i = 0; i < n; ++i) {
		bTree.erase(getKey<T>(i));
	}

	if (bTree.size() != 0) {
		cerr << "damn, bTree is not empty after deletion" << endl;
	}
	assert(bTree.size() == 0);
	*/
}

static int launchSchematest(char** argv) {
	schemaTest(argv[2], argv[3]);
	return 0;
}

}

