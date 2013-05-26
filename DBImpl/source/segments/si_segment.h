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

	// mapping of all segments, key is segmentId
	std::unordered_map<uint64_t, Segment*> segMapping;

public:
	SISegment(std::vector<uint64_t> extents, uint64_t segId);

	void addToMap(std::pair<uint64_t, Segment*> segment);
	Segment* retrieveFromMap(uint64_t segId);
	void removeFromMap(uint64_t segId);
};



#endif /* SI_SEGMENT_H_ */
