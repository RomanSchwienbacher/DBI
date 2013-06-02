#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "../segments/segman.h"
#include "../segments/btree_segment.h"
#include "../index/btree.h"

using namespace std;

namespace testing {

/* Comparator functor for uint64_t*/
struct MyCustomUInt64Cmp {
	bool operator()(uint64_t a, uint64_t b) const {
		return a < b;
	}
};

template<unsigned len>
struct Char {
	char data[len];
};

/* Comparator functor for char */
template<unsigned len>
struct MyCustomCharCmp {
	bool operator()(const Char<len>& a, const Char<len>& b) const {
		return memcmp(a.data, b.data, len) < 0;
	}
};

typedef std::pair<uint32_t, uint32_t> IntPair;

/* Comparator for IntPair */
struct MyCustomIntPairCmp {
	bool operator()(const IntPair& a, const IntPair& b) const {
		if (a.first < b.first)
			return true;
		else
			return (a.first == b.first) && (a.second < b.second);
	}
};

template<class T>
static const T& getKey(const uint64_t& i);

template<>
const uint64_t& getKey(const uint64_t& i) {
	return i;
}

static std::vector<std::string> char20;

template<>
const Char<20>& getKey(const uint64_t& i) {
	std::stringstream ss;
	ss << i;
	std::string s(ss.str());
	char20.push_back(std::string(20 - s.size(), '0') + s);
	return *reinterpret_cast<const Char<20>*>(char20.back().data());
}

static std::vector<IntPair> intPairs;

template<>
const IntPair& getKey(const uint64_t& i) {
	intPairs.push_back(std::make_pair(i / 3, 3 - (i % 3)));
	return intPairs.back();
}

template<class T, class CMP>
static void bTreeTest(const std::string& filename, uint64_t n) {

	// Set up stuff, you probably have to change something here to match to your interfaces
	//BufferManager bm("/tmp/db", 10ul * 1024ul * 1024ul); // bogus arguments -> 40 GB?!?!?!
	BufferManager bm(filename, 25600ul); // bogus arguments -> 100 MB
	SegmentManager sm(1, 1, &bm);
	uint64_t spId = sm.createSegment(SegmentType::BTREE, 10);
	BTreeSegment& seg = (BTreeSegment&) (sm.getSegment(spId));
	BTree<T, CMP> bTree(seg);

	cout << "Insert values" << endl;
	for (uint64_t i = 0; i < n; ++i) {

		//bTree.insert(getKey<T>(i), static_cast<TID>(i * i));

		TID tid;
		tid.pageId = i * i;
		tid.slotId = 0; // TODO fix this: how to get page-id and slot-id by simple i increment? bit-shifting?
		bTree.insert(getKey<T>(i), tid);
	}
	assert(bTree.size() == n);

	cout << "Check if they can be retrieved" << endl;
	for (uint64_t i = 0; i < n; ++i) {
		TID tid;
		assert(bTree.lookup(getKey<T>(i), tid));

		//assert(tid == i * i);
		assert(tid.pageId == i * i); // TODO fix this: how to get page-id and slot-id by simple i increment? bit-shifting?
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
			assert(!bTree.lookup(getKey<T>(i), tid));
		} else {
			assert(bTree.lookup(getKey<T>(i), tid));

			//assert(tid == i * i);
			assert(tid.pageId == i * i); // TODO fix this: how to get page-id and slot-id by simple i increment? bit-shifting?
		}
	}

	cout << "Delete everything" << endl;
	for (uint64_t i = 0; i < n; ++i) {
		bTree.erase(getKey<T>(i));
	}

	assert(bTree.size() == 0);
}

static int launchBTreetest(char** argv) {

	// Get command line argument (amount of tid's managed by tree)
	const uint64_t n = strtoul(argv[9], NULL, 10);

	cout << "Test index with 64bit unsigned integers" << endl;
	bTreeTest<uint64_t, MyCustomUInt64Cmp>(argv[4], n);

	cout << "Test index with 20 character strings" << endl;
	bTreeTest<Char<20>, MyCustomCharCmp<20>>(argv[4], n);

	cout << "Test index with compound key" << endl;
	bTreeTest<IntPair, MyCustomIntPairCmp>(argv[4], n);

	return 0;
}

}
