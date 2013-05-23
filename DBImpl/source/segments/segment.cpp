/*
 * segment.cpp
 *
 *  Created on: May 15, 2013
 *      Author: dave
 */

#include <algorithm>
#include "segment.h"

Segment::Segment(uint64_t segId){
	Segment::segId = segId;
	size = 1;
}

Segment::Segment(std::vector<uint64_t> extents, uint64_t segId){

	Segment::segId = segId;
	Segment::extents = extents;

	size = calculateSize(extents);
}

void Segment::grow(std::vector<uint64_t> addExtents){

	extents= mergeExtents(extents, addExtents);
	size = calculateSize(extents);

}

uint64_t Segment::calculateSize(std::vector<uint64_t> extents){

	uint64_t ret = 0;

	for(int i = 0; i < extents.size(); i+=2){
		ret += (extents.at(i+1) - extents.at(i));
	}

	return ret;
}

std::vector<uint64_t> Segment::mergeExtents(std::vector<uint64_t> extents1, std::vector<uint64_t> extents2){

	// append second extent vector to first one
	extents1.insert(extents1.end(), extents2.begin(), extents2.end());

	// sort entire extent vector
	std::sort(extents1.begin(), extents1.end());

	// merge neighboring extents
	for(int i = 1; i< extents1.size(); i+=2){
		if(extents1.at(i) == extents1.at(i+1)){
			extents1.erase(extents1.begin() + i);
			extents1.erase(extents1.begin() + i + 1);
		}
	}

	return extents1;
}

uint64_t Segment::getSize(){

	return size;
}

std::vector<uint64_t> Segment::getExtents(){

	return extents;

}
