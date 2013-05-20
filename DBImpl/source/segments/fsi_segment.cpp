/*
 * fsi_segment.cpp
 *
 *  Created on: May 20, 2013
 *      Author: dave
 */


#include "fsi_segment.h"


FSISegment::FSISegment(uint64_t size, uint64_t segId, uint64_t maxPageId){

	FSISegment::segId = segId;
	FSISegment::size = 1;

	extent freeExtent;
	freeExtent.min = segId + 1;
	freeExtent.max = maxPageId + 1;

	extent thisSegment;
	thisSegment.min = 1;
	thisSegment.max = 2;

	// internal record of how many extents the FSI segment spans
	extents.push_back(thisSegment);

	// record of all free extents in DB
	freeExtents.push_back(freeExtent);

}

std::vector<Segment::extent> FSISegment::getFreePages(uint64_t size){

	std::vector ret;

	// iterate through free extents and find
	for(int i = 0; i< freeExtents.size(); ++i){


	}

}
