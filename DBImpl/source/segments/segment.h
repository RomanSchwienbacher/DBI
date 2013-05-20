/*
 * segment.h
 *
 *  Created on: May 15, 2013
 *      Author: dave
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <stdint.h>
#include <vector>

class Segment{

protected:

	// SegmentId
	uint64_t segId;

	// Size of this segment in pages
	uint64_t size;

	// structure with min PageID (inclusive) and max PageId (exclusive) of extent
	struct extent {
				uint64_t min, max; // minimum and maximum pageId of extent
		};

	// vector of extents
	std::vector<extent> extents; // array of the extents within this Segment

public:

	Segment(){};

	Segment(std::vector<extent>, uint64_t segId);

	Segment& grow(uint64_t newSize);

	// returns size in pages
	uint64_t getSize();

	uint64_t getId();

	// Iteration
	char* nextPage();
	char* prevPage();

};


#endif /* SEGMENT_H_ */
