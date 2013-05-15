/*
 * segment.h
 *
 *  Created on: May 15, 2013
 *      Author: dave
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

class Segment{

	struct {
		uint64_t min, max; // minimum and maximum pageId of extent
	} extent;

	uint64_t extentCount; // number of continuous extents used by the segment
	extent allExtents[]; // array of all extents within the Segment

public:

	Segment(uint64_t size);

	Segment& grow(uint64_t newSize);

	// returns size in pages
	uint64_t size();

	// Iteration
	char* nextPage();
	char* prevPage();


};


#endif /* SEGMENT_H_ */
