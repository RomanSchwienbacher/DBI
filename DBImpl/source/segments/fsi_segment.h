/*
 * fsi_segment.h
 *
 *  Created on: May 20, 2013
 *      Author: dave
 */

#ifndef FSI_SEGMENT_H_
#define FSI_SEGMENT_H_

#include "segment.h"

class FSISegment : private Segment{

	std::vector<extent> freeExtents; // array of all free extents in db


public:

	FSISegment(uint64_t size, uint64_t segId, uint64_t maxPageId);

	std::vector<Segment::extent> getFreeExtents(uint64_t size);

};


#endif /* FSI_SEGMENT_H_ */
