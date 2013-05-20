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

/**
 * this method is used by the si segment to acquire the necessary
 * extents for a new segment
 *
 * @param size: the amount of pages needed to be in the segment
 * @return: empty vector if not enough pages available, vector of extents otherwise
 */

std::vector<Segment::extent> FSISegment::getFreeExtents(uint64_t size){

	std::vector<extent> ret;
	uint64_t neededPages = size;
	extent currentExtent;

	// iterate through free extents
	for(int i = 0; i< freeExtents.size(); ++i){

		currentExtent = freeExtents.at(i);

		if((currentExtent.max - currentExtent.min) >= neededPages){

			// remove free extent
			freeExtents.erase(freeExtents.begin() + i);

			uint64_t oldMax = currentExtent.max;

			// adjust currentExtent to match needed size
			currentExtent.max = currentExtent.min + size;

			// place free extent in return
			ret.push_back(currentExtent);

			// place rest of extent back into freeExtents
			if(oldMax != currentExtent.max){

				currentExtent.min = currentExtent.max;
				currentExtent.max = oldMax;
				freeExtents.insert(freeExtents.begin() + i, currentExtent);
			}

			neededPages = 0;
			break;

		} else if((currentExtent.max - currentExtent.min) < neededPages){

			// place extent in return
			ret.push_back(currentExtent);

			// remove free extent
			freeExtents.erase(freeExtents.begin() + i);

			neededPages -= (currentExtent.max - currentExtent.min);
		}
	}

	if(neededPages != 0){
		ret.clear();
	}

	return ret;

}
