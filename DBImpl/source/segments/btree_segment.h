/*
 * btree_segment.h
 *
 *  Created on: June 2, 2013
 *      Author: DRomanAvid
 */

#ifndef BTREE_SEGMENT_H_
#define BTREE_SEGMENT_H_

#include "segment.h"
#include "tid.h"

using namespace std;

class BTreeSegment: public Segment {


public:

	BTreeSegment(vector<uint64_t> freeExtents, uint64_t segId, FSISegment *fsi, BufferManager * bm);

	virtual ~BTreeSegment();
};

#endif
