/*
 * sp_segment.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef SP_SEGMENT_H_
#define SP_SEGMENT_H_

#include "segment.h"
#include "tid.h"
#include "record.h"

class SPSegment: private Segment {


public:
	SPSegment();

	TID insert(const Record& r);

	bool remove(TID tid);

	Record* lookup(TID tid);

	bool update(TID tid, const Record& r);

	virtual ~SPSegment();
};

#endif
