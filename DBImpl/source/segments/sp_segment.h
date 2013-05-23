/*
 * sp_segment.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef SP_SEGMENT_H_
#define SP_SEGMENT_H_

#include "segment.h"
#include "tid.cpp"
#include "record.h"
#include "slottedpage.h"

using namespace std;

class SPSegment: private Segment {

	// managed slotted pages by segment
	vector<SlottedPage*> slottedPages;

public:
	SPSegment(uint64_t segId);

	TID insert(const Record& r);

	bool remove(TID tid);

	Record* lookup(TID tid);

	bool update(TID tid, const Record& r);

	virtual ~SPSegment();
};

#endif
