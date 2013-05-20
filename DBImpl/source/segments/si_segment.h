/*
 * si_segment.h
 *
 *  Created on: May 15, 2013
 *      Author: dave
 */

#ifndef SI_SEGMENT_H_
#define SI_SEGMENT_H_

#include <unordered_map>
#include "segment.h"
#include "fsi_segment.h"

class SISegment : private Segment{

	uint64_t createSegment(uint64_t size);
	int dropSegment(uint64_t segId);
	Segment& growSegment(uint64_t segId, uint64_t newSize);
	Segment& getSegment(uint64_t segId);

	// mapping of all segments, key is segmentId
	std::unordered_map<uint64_t, Segment> segMapping;

	// FSI Segment: here is where the information about free segments is kept
	FSISegment *fsiSeg;

	// id counter used for creating segment IDs
	uint64_t currentId = 0;

public:
	SISegment(uint64_t maxPageId);

};



#endif /* SI_SEGMENT_H_ */
