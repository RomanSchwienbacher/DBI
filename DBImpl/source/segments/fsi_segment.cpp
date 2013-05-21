/*
 * fsi_segment.cpp
 *
 *  Created on: May 20, 2013
 *      Author: dave
 */

#include "fsi_segment.h"


FSISegment::FSISegment(uint64_t size, uint64_t segId, uint64_t maxPageId) : Segment(segId){

	FSISegment::size = size;

	uint64_t endOfFSI = size +1;

	// internal record of how many extents the FSI segment spans
	extents.push_back(1);
	extents.push_back(endOfFSI);

	// record of all free extents in DB
	freeExtents.push_back(endOfFSI);
	freeExtents.push_back(maxPageId +1);

}

/**
 * this method is used by the si segment to acquire the necessary
 * extents for a new segment
 *
 * @param size: the amount of pages needed to be in the segment
 * @return: empty vector if not enough pages available, vector of extents otherwise
 */

std::vector<uint64_t> FSISegment::getFreeExtents(uint64_t size){

	std::vector<uint64_t> ret;
	uint64_t neededPages = size;
	uint64_t max = 0;
	uint64_t min = 0;

	// iterate through free extents
	for(int i = 0; i< freeExtents.size(); i+=2){

		max = freeExtents.at(i+1);
		min = freeExtents.at(i);

		if((max - min) >= neededPages){

			uint64_t oldMax = max;
			uint64_t neededMax = min + neededPages;

			// place free extent in return
			ret.push_back(min);
			ret.push_back(neededMax);

			// remove free extent
			freeExtents.erase(freeExtents.begin() + i);
			freeExtents.erase(freeExtents.begin() + i + 1);

			// place rest of extent back into freeExtents
			if(oldMax != neededMax){

				freeExtents.insert(freeExtents.begin() + i, neededMax);
				freeExtents.insert(freeExtents.begin() + i +1, oldMax);
			}

			neededPages = 0;
			break;

		} else if((max - min) < neededPages){

			// place extent in return
			ret.push_back(min);
			ret.push_back(max);

			// remove free extent
			freeExtents.erase(freeExtents.begin() + i);
			freeExtents.erase(freeExtents.begin() + i +1);

			neededPages -= (max - min);
		}
	}

	if(neededPages != 0){
		ret.clear();
	}

	return ret;

}

void FSISegment::returnFreeExtents(std::vector<uint64_t> freedExtents){

	freeExtents = mergeExtents(freeExtents, freedExtents);

}
