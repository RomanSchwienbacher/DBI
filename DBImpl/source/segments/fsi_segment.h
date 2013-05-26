/*
 * fsi_segment.h
 *
 *  Created on: May 20, 2013
 *      Author: dave
 */

#ifndef FSI_SEGMENT_H_
#define FSI_SEGMENT_H_

#include <vector>
#include "segment.h"

class FSISegment : private Segment{

	// array of all free extents in db
	std::vector<uint64_t> freeExtents;


public:

	FSISegment(std::vector<uint64_t> extents, uint64_t segId, uint64_t maxPageId);

	std::vector<uint64_t> getFreeExtents(uint64_t size);
	void returnFreeExtents(std::vector<uint64_t> freedExtents);

};


#endif /* FSI_SEGMENT_H_ */
