/*
 * btree_segment.cpp
 *
 *	Segment type BTreeSegments that operates on B+Trees
 *
 *	 Created on: June 2, 2013
 *      Author: DRomanAvid
 */

#include <cstring>
#include <iostream>
#include <sstream>
#include "btree_segment.h"
#include "fsi_segment.h"
#include "../buffer/bufframe.h"
#include <stdexcept>

using namespace std;

/**
 * Constructor:
 */
BTreeSegment::BTreeSegment(vector<uint64_t> freeExtents, uint64_t segId, FSISegment *fsi, BufferManager * bm) :
		Segment(freeExtents, segId, fsi, bm) {
}

/**
 * Grows and returns one new page-id
 */
uint64_t BTreeSegment::getNextPageId() {

	// 1st step: grow by 1
	vector<uint64_t> newExtents = grow(1);

	// 2nd step: return fetched pageId
	return newExtents.front();
}

BTreeSegment::~BTreeSegment() {

}

