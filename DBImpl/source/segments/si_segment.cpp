/*
 * si_segment.cpp
 *
 * Created on: May 20, 2013
 * Author: dave
 */

#include <utility>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "si_segment.h"

SISegment::SISegment(std::vector<uint64_t> extents, uint64_t segId) : Segment(extents, segId) {
		
	// DEBUG
		std::cout << "SISegment extents: "<< std::endl;
		for(int i = 0; i < extents.size(); ++i){
			std::cout << "value: " << i << ": " << extents.at(i) << std::endl;
		}


	// TODO: retrieve segment Mapping from disk
	// or let it be handed by parameter from segman
}

/**
 * inserts a segID, segment* pair into the segment mapping
 * @param segment: pair of segId and segment pointer
 */
void SISegment::addToMap(std::pair<uint64_t, Segment*> segment) {
	segMapping.insert(segment);
}

/**
 * retrieves a segment pointer from the segment map in the inventory
 * @param segId: id of the segment to be retrieved
 * @return: pointer to the segment to be retrieved
 */
Segment* SISegment::retrieveFromMap(uint64_t segId) {

	return segMapping.at(segId);
}

/**
 * removes segment from the mapping
 * @param segId: id of the segment to be removed from the hashmap
 */
void SISegment::removeFromMap(uint64_t segId) {
	segMapping.erase(segId);
}
