/*
 * fsi_segment.cpp
 *
 *  Created on: May 20, 2013
 *      Author: dave
 */

#include <iostream>
#include "fsi_segment.h"


FSISegment::FSISegment(std::vector<uint64_t> extents, uint64_t segId, uint64_t maxPageId) : Segment(extents, segId){

	//TODO: +1 must be changed if si segment > 1 page
	uint64_t endOfFSI = size +1;

	// record of all free extents in DB
	freeExtents.push_back(endOfFSI);
	freeExtents.push_back(maxPageId +1);

	// DEBUG
	std::cout << "FSISegment extents: "<< std::endl;
	for(int i = 0; i < extents.size(); ++i){
		std::cout << "value: " << i << ": " << extents.at(i) << std::endl;
	}
	std::cout << "FSISegment FREE extents: "<< std::endl;
		for(int i = 0; i < freeExtents.size(); ++i){
			std::cout << "value: " << i << ": " << freeExtents.at(i) << std::endl;
		}

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
	std::vector<uint64_t> newFreeExtents;
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

			// place rest of extent back into freeExtents
			if(oldMax != neededMax){

				newFreeExtents.push_back(neededMax);
				newFreeExtents.push_back(oldMax);
			}

			neededPages = 0;
			break;

		} else if((max - min) < neededPages){

			// place extent in return
			ret.push_back(min);
			ret.push_back(max);

			neededPages -= (max - min);
		}
	}

	if(neededPages != 0){
		ret.clear();
	}

	freeExtents = newFreeExtents;

	return ret;

}

void FSISegment::returnFreeExtents(std::vector<uint64_t> freedExtents){

	freeExtents = mergeExtents(freeExtents, freedExtents);

}
