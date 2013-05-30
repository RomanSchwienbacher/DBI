/*
 * segment.cpp
 *
 *  Created on: May 15, 2013
 *      Author: dave
 */

#include <algorithm>
#include <iostream>
#include "segment.h"

/**
 * Constructor for Segment Inventory and Free Segment Inventory
 * @param extents: extents used for the segment
 * @param segId: id of the segment
*/
Segment::Segment(std::vector<uint64_t> extents, uint64_t segId) {
	Segment::segId = segId;
	Segment::extents = extents;
	size = calculateSize(extents);
}

/**
 * Constructor for Regular segments
 * @param extents: extents used for the segment
 * @param segId: id of the segment
 * @param bm: buffer manager for writing to disk
*/
Segment::Segment(std::vector<uint64_t> extents, uint64_t segId, BufferManager * bm) {

	Segment::segId = segId;
	Segment::extents = extents;
	Segment::bm = bm;
	size = calculateSize(extents);
	//Segment::expandExtents(extents);
}

void Segment::expandExtents(std::vector<uint64_t> extents){

	allPages.clear();

	uint64_t pageId;

	uint64_t diff;
	for(unsigned i = 1; i < extents.size(); i+=2){
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
	for (unsigned i = 0; i < extents.size(); i += 2) {
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
	for (unsigned i = 1; i < extents1.size(); i += 2) {
		if (i+1 == extents1.size()){
			mergedExtents.push_back(extents1.at(i));
			break;
		}
		if (extents1.at(i) != extents1.at(i + 1)) {
			mergedExtents.push_back(extents1.at(i));
			mergedExtents.push_back(extents1.at(i+1));
		}
	}

	return mergedExtents;
}

uint64_t Segment::at(uint64_t pos){

	uint64_t ret;

	if(pos >= size){
		return -1;
	}

	uint64_t offset = 0;
	int currentExt = 0;
	uint64_t extRange = 0;
	uint64_t prevExtRange = 0;

	for(unsigned i = 0; i < extentLengths.size(); ++i){
		prevExtRange = extRange;
		extRange += extentLengths.at(i);

		if(pos < extRange){
			currentExt = i;
			offset = pos - prevExtRange;
			break;
		}
	}

	ret = extents.at(currentExt * 2) + offset;

	return ret;
}

uint64_t Segment::getSize(){

	return size;

}

std::vector <uint64_t> Segment::getExtents(){
	return extents;
}
