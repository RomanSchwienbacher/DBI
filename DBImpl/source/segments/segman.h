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
	SISegment *segmentInventory;

	// FSI Segment: here is where the information about free segments is kept
	FSISegment *freeSegmentInventory;

	// id counter used for creating segment IDs
	uint64_t currentId = 0;

public:
	SegmentManager(uint64_t si_size, uint64_t fsi_size, BufferManager* bm);

	uint64_t createSegment(uint64_t size);
	Segment& getSegment(uint64_t segId);
	void dropSegment(uint64_t segId);
	uint64_t growSegment(uint64_t segId, uint64_t newSize);

	~SegmentManager();
};

#endif
