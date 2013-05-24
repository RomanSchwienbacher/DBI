/*
 * segment.cpp
 *
 *  Created on: May 15, 2013
 *      Author: dave
 */

#include <algorithm>
#include "segment.h"

Segment::Segment(uint64_t segId) {
	Segment::segId = segId;
	size = 1;
}

Segment::Segment(std::vector<uint64_t> extents, uint64_t segId) {

	Segment::segId = segId;
	Segment::extents = extents;
	size = calculateSize(extents);
	//Segment::expandExtents(extents);
}

void Segment::expandExtents(std::vector<uint64_t> extents){

	allPages.clear();

	uint64_t pageId;

	uint64_t diff;
	for(int i = 1; i < extents.size(); i+=2){
		pageId = extents.at(i-1);
		diff = extents.at(i+1) - extents.at(i);

		while(0 < diff--){
			allPages.push_back(pageId++);
		}
	}
}

std::vector<uint64_t> Segment::grow(std::vector<uint64_t> addExtents) {

	extents = mergeExtents(extents, addExtents);
	size = calculateSize(extents);

	return addExtents;

}

uint64_t Segment::calculateSize(std::vector<uint64_t> extents) {

	uint64_t ret = 0;

	extentLengths.clear();
	for (int i = 0; i < extents.size(); i += 2) {
		ret += (extents.at(i + 1) - extents.at(i));
		extentLengths.push_back((extents.at(i+1) - extents.at(i)));
	}

	return ret;
}

std::vector<uint64_t> Segment::mergeExtents(std::vector<uint64_t> extents1,
		std::vector<uint64_t> extents2) {

	// append second extent vector to first one
	extents1.insert(extents1.end(), extents2.begin(), extents2.end());

	// sort entire extent vector
	std::sort(extents1.begin(), extents1.end());

	// create mergedExtents and fill with first value
	std::vector<uint64_t> mergedExtents;
	mergedExtents.push_back(extents1.at(0));

	// merge neighboring extents
	for (int i = 1; i < extents1.size(); i += 2) {
		if (extents1.at(i) != extents1.at(i + 1)) {
			mergedExtents.push_back(extents1.at(i));
		}
	}

	return mergedExtents;
}

uint64_t Segment::at(uint64_t pos){

	uint64_t ret;

	if(pos >= size){
		return -1;
	}

//	return allPages.at(pos);

	for(int i = 0; i < extentLengths.size(); ++i){
		pos -= extentLengths.at(i);
		if(pos <=0){
			ret = extents.at((2 * (i+1)) - 1) + pos - 1;
			break;
		}
	}

	return ret;
}

uint64_t Segment::getSize(){

	return size;

}

std::vector <uint64_t> Segment::getExtents(){
	return extents;
}
