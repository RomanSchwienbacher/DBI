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

class Segment {

protected:

	// SegmentId
	uint64_t segId;

	// Size of this segment in pages
	uint64_t size;

	// list of extents
	std::vector<uint64_t> extents;

	// calculate the size in pages from the extent vector
	uint64_t calculateSize(std::vector<uint64_t> extents);

	// merge extents
	std::vector<uint64_t> mergeExtents(std::vector<uint64_t> extents1,
			std::vector<uint64_t> extents2);

public:

	Segment(uint64_t segId);

	Segment(std::vector<uint64_t> extents, uint64_t segId);

	void grow(std::vector<uint64_t> addExtents);

	// returns size in pages
	uint64_t getSize();

	uint64_t getId();

	std::vector<uint64_t> getExtents();

	// Iteration
	char* nextPage();
	char* prevPage();

};

#endif /* SEGMENT_H_ */
