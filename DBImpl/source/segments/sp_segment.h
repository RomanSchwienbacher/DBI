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

class SPSegment: private Segment {

	// managed slotted pages identified by page-id
	unordered_map<uint64_t, SlottedPage*> slottedPagesMap;

public:
	SPSegment(uint64_t segId);

	TID insert(const Record& r);

	bool remove(TID tid);

	Record* lookup(TID tid);

	bool update(TID tid, const Record& r);

	virtual ~SPSegment();
};

#endif
