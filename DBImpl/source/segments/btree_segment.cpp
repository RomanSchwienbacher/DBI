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
BTreeSegment::BTreeSegment(vector<uint64_t> freeExtents, uint64_t segId, BufferManager * bm) :
		Segment(freeExtents, segId, bm) {


}

BTreeSegment::~BTreeSegment() {

}

