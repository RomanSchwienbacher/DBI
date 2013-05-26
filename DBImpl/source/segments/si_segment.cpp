/*
 * si_segment.cpp
 *
 *  Created on: May 20, 2013
 *      Author: dave
 */

#include <utility>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "si_segment.h"
#include "sp_segment.h"

SISegment::SISegment(uint64_t maxPageId) : Segment(0){

	// increment by one for SISegment
	currentId++;

	// create FSISegment and increment currentId to 2
	fsiSeg = new FSISegment(1, currentId++, maxPageId);

	extents.push_back(0);
	extents.push_back(1);

}

/**
 * creates a new segment
 * @param size: the size in pages of the new segment to be created
 * @return: segmentId of new segment, -1 if not enough free space
 */

uint64_t SISegment::createSegment(uint64_t size){

	uint64_t ret;

	std::vector<uint64_t> freeExtents;

	freeExtents = fsiSeg->getFreeExtents(size);

	// getFreeExtents returns size 0 if not enough free extents exist
	if(freeExtents.size() == 0){
		exit(-1);
	} else{
		ret = currentId;
	}

	// TODO consider different segment types for init
	Segment* seg = new SPSegment(freeExtents, currentId, bm);
	segMapping.insert(std::make_pair(currentId++, seg));

	return ret;

}

/**
 * retrieves a segment
 *
 * @param segId: id of the segment to be retrieved
 * @return: a reference to the segment
 */

Segment& SISegment::getSegment(uint64_t segId){

	Segment* ret;

	try{
		 ret = segMapping.at(segId);
	}
	catch (const std::out_of_range& oor){
		std::cerr << "** Segment with id: " << segId << " not in map **" << std::endl;
		exit(-1);
	}

	return *ret;
}

/**
 * grows a segment to a new size of pages
 *
 * @param segId: the segment Id of the segment to be grown
 * @param newSize: the new size the segment is supposed to be extended to
 * @return: current size of the segment in pages (success), -1 if resize failed
 */
uint64_t SISegment::growSegment(uint64_t segId, uint64_t newSize){

	// retrieve segment
	Segment& seg = getSegment(segId);

	// if newSize is old size then do nothing and return that size
	if(seg.getSize() == newSize){
		return newSize;
	} else if(seg.getSize() > newSize){
		// if newSize is smaller, then return an error
		return -1;
	}

	// try to get enough free extents for growing
	std::vector<uint64_t> addExtents = fsiSeg->getFreeExtents(newSize - seg.getSize());

	seg.grow(addExtents);

	return seg.getSize();
}

/**
 * drops a segment from the database
 *
 * @param segId: id of segment to be dropped
 *
 */

void SISegment::dropSegment(uint64_t segId){

	int ret;

	// retrieve segment
	Segment& seg = getSegment(segId);

	// return extents to free space
	fsiSeg->returnFreeExtents(seg.getExtents());


	// remove segment from mapping
	segMapping.erase(segId);
}

