/*
 * segman.cpp
 *
 *	Contains the SegmentManager
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#include "segman.h"
#include "sp_segment.h"
#include "btree_segment.h"
#include "sch_segment.h"
#include <iostream>

SegmentManager::SegmentManager(uint64_t si_size, uint64_t fsi_size, BufferManager* bm) {

	SegmentManager::bm = bm;

	currentId = 0;

	// create SISegment of size si_size
	std::vector<uint64_t> siExtents;
	siExtents.push_back(0);
	siExtents.push_back(si_size);
	segmentInventory = new SISegment(siExtents, currentId++);

	// create FSISegment and increment currentId to 2
	std::vector<uint64_t> fsiExtents;
	fsiExtents.push_back(si_size);
	fsiExtents.push_back(si_size + fsi_size);

	freeSegmentInventory = new FSISegment(fsiExtents, currentId++, bm->getPagesOnDisk());

}

/**
 * creates a new segment
 * @param type: the type of the created segment
 * @param size: the size in pages of the new segment to be created
 * @param schema: the filename of the db-schema (for SCHEMA segments only)
 * @return: segmentId of new segment, -1 if not enough free space
 */
uint64_t SegmentManager::createSegment(SegmentType type, uint64_t size, const std::string& schema) {

	int ret;

	std::vector<uint64_t> freeExtents;

	freeExtents = freeSegmentInventory->getFreeExtents(size);

	// getFreeExtents returns size 0 if not enough free extents exist
	if (freeExtents.size() == 0) {
		ret = -1;
	} else {
		ret = currentId;
	}

	if (type == SegmentType::SLOTTED_PAGE) {

		Segment* seg = new SPSegment(freeExtents, currentId, freeSegmentInventory, bm);
		segmentInventory->addToMap(std::make_pair(currentId++, seg));

	} else if (type == SegmentType::BTREE) {

		Segment* seg = new BTreeSegment(freeExtents, currentId, freeSegmentInventory, bm);
		segmentInventory->addToMap(std::make_pair(currentId++, seg));

	} else if (type == SegmentType::SCHEMA) {

		Segment* seg = new SchemaSegment(freeExtents, currentId, freeSegmentInventory, bm, schema);
		segmentInventory->addToMap(std::make_pair(currentId++, seg));
	}

	return ret;

}

/**
 * retrieves a segment
 *
 * @param segId: id of the segment to be retrieved
 * @return: a reference to the segment
 */

Segment& SegmentManager::getSegment(uint64_t segId) {

	Segment* ret;

	try {
		ret = segmentInventory->retrieveFromMap(segId);
	} catch (const std::out_of_range& oor) {
		std::cerr << "** Segment with id: " << segId << " not in map - creating one **" << std::endl;
		uint64_t oldCurrentId = currentId;
		currentId = segId;
		createSegment(SegmentType::SLOTTED_PAGE, 10, NULL);
		currentId = oldCurrentId;
		return getSegment(segId);
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
uint64_t SegmentManager::growSegment(uint64_t segId, uint64_t newSize) {

	// retrieve segment
	Segment& seg = getSegment(segId);

	// if newSize is old size then do nothing and return that size
	if (seg.getSize() == newSize) {
		return newSize;
	} else if (seg.getSize() > newSize) {
		// if newSize is smaller, then return an error
		return -1;
	}

	seg.grow(newSize - seg.getSize());

	return seg.getSize();
}

/**
 * drops a segment from the database
 *
 * @param segId: id of segment to be dropped
 *
 */

void SegmentManager::dropSegment(uint64_t segId) {

	int ret;

	// retrieve segment
	Segment& seg = getSegment(segId);

	// return extents to free space
	freeSegmentInventory->returnFreeExtents(seg.getExtents());

	// remove segment from mapping
	segmentInventory->removeFromMap(segId);
}

SegmentManager::~SegmentManager() {
}

