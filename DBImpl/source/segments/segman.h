/*
 * segman.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef SEGMAN_H_
#define SEGMAN_H_

#include "../buffer/bufman.h"
#include "si_segment.h"

class SegmentManager {

	BufferManager* bm;
	SISegment* segmentInventory;

public:
	SegmentManager(BufferManager* bm);

	uint64_t createSegment(uint64_t size);
	Segment& getSegment(uint64_t segId);

	virtual ~SegmentManager();
};

#endif
