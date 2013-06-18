/*
 * segment.h
 *
 *  Created on: May 15, 2013
 *      Author: dave
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include "../buffer/bufman.h"
//#include "fsi_segment.h"
#include <stdint.h>
#include <vector>

class FSISegment;

class Segment {

protected:

	// SegmentId
	uint64_t segId;

	// Size of this segment in pages
	uint64_t size;

	// list of extents
	std::vector<uint64_t> extents;

	// list of extent lengths
	std::vector<uint64_t> extentLengths;

	// list of all pageIds
	std::vector<uint64_t> allPages;

	// calculate the size in pages from the extent vector
	uint64_t calculateSize(std::vector<uint64_t> extents);

	// merge extents
	std::vector<uint64_t> mergeExtents(std::vector<uint64_t> extents1,
			std::vector<uint64_t> extents2);

	// expand extents to list all pageIds
	void expandExtents(std::vector<uint64_t> extents);

	// pointer to the buffer manager of the db system
	BufferManager * bm;

	// pointer to the free segment inventory (used for growing)
	FSISegment *fsi;

public:

	Segment(std::vector<uint64_t> extents, uint64_t segId);

	Segment(std::vector<uint64_t> extents, uint64_t segId, FSISegment *fsi, BufferManager * bm);

	std::vector<uint64_t> grow(uint64_t addedSpace);

	// returns size in pages
	uint64_t getSize();

	uint64_t getId();

	BufferManager* getBm();

	std::vector<uint64_t> getExtents();

	// Iteration
	char* nextPage();
	char* prevPage();

	uint64_t at(uint64_t pos);

};

// Defines which segment types can be created
enum class SegmentType {SLOTTED_PAGE, BTREE, SCHEMA};

#endif
