/*
 * sp_segment.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef SP_SEGMENT_H_
#define SP_SEGMENT_H_

#include <unordered_map>
#include "segment.h"
#include "tid.cpp"
#include "record.h"
#include "slottedpage.h"

using namespace std;

class SPSegment: public Segment {

	// managed slotted pages identified by page-id
	unordered_map<uint64_t, SlottedPage*> slottedPagesMap;

public:
	SPSegment(vector<uint64_t> freeExtents, uint64_t segId, BufferManager * bm);

	TID insert(const Record& r);

	bool remove(TID tid);

	const Record* lookup(TID tid);

	bool update(TID tid, const Record& r);

	virtual ~SPSegment();
};

#endif
