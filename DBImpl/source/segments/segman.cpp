/*
 * segman.cpp
 *
 *	Contains the SegmentManager
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#include "segman.h"

SegmentManager::SegmentManager(BufferManager* bm) {

	SegmentManager::bm = bm;

	// TODO initialize/load segment inventory properly
	SegmentManager::segmentInventory = new SISegment(0);
}

uint64_t SegmentManager::createSegment(uint64_t size) {
	return segmentInventory->createSegment(size);
}

Segment& SegmentManager::getSegment(uint64_t segId) {
	return segmentInventory->getSegment(segId);
}

void SegmentManager::dropSegment(uint64_t segId){
	segmentInventory->dropSegment(segId);
}

uint64_t SegmentManager::growSegment(uint64_t segId, uint64_t newSize){
	return segmentInventory->growSegment(segId, newSize);
}

SegmentManager::~SegmentManager() {
}

