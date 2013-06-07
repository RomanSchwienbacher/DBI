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
#include <unistd.h>
#include <string.h>
#include "si_segment.h"

SISegment::SISegment(std::vector<uint64_t> extents, uint64_t segId) :
		Segment(extents, segId) {

	// DEBUG
	std::cout << "SISegment extents: " << std::endl;
	for (unsigned i = 0; i < extents.size(); ++i) {
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
 * flushes the mapping of segments to disk
 */
void SISegment::flushToDisk() {

	// prepare size pages
	char *temp = new char[size * sysconf(_SC_PAGESIZE)];
	int offset = 0;

	uint64_t segId = 0;
	Segment *segment;

	unordered_map<uint64_t, Segment *>::size_type mappingSize = 0;
	std::vector<uint64_t>::size_type extentSize = 0;

	// serialize map size
	mappingSize = segMapping.size();
	memcpy(temp + offset, &(mappingSize), sizeof(unordered_map<uint64_t, Segment *>::size_type));
	offset += sizeof(unordered_map<uint64_t, Segment *>::size_type);

	// serialize mapping
	for (auto it = segMapping.begin(); it != segMapping.end(); ++it) {

		segId = it->first;
		segment = it->second;

		// serialize segmendId
		memcpy(temp + offset, &(segId), sizeof(uint64_t));
		offset += sizeof(uint64_t);

		// serialize vector length
		extentSize = segment->getExtents().size();
		memcpy(temp + offset, &(extentSize), sizeof(std::vector<uint64_t>::size_type));
		offset += sizeof(std::vector<uint64_t>::size_type);

		//serialize vector data = extents min and max
		for (unsigned i = 0; i < segment->getExtents().size(); ++i) {
			memcpy(temp + offset, &(segment->getExtents().at(i)), sizeof(uint64_t));
			offset += sizeof(uint64_t);
		}

	}

	// fix size pages from bm
	// write pages to bm
	for (unsigned i = 0; i < size; ++i) {
		BufferFrame frame = bm->fixPage(at(i), true);

		try {

			// write into frame data pointer
			memcpy(frame.getData(), temp + (i * sysconf(_SC_PAGESIZE)), bm->getPageSize());

		} catch (exception& e) {
			cerr << "An exception occurred while writing slotted page to frame: " << e.what() << endl;
		}
	}

	delete temp;

}

/**
 * retrieves the mapping of segments from disk
 */
void SISegment::readFromDisk() {

	int pageSize = bm->getPageSize();

	// prepare size pages
	char *temp = new char[size * pageSize];
	int offset = 0;

	// clear current segment mapping
	segMapping.clear();

	// read entire segment inventory from disk
	for (unsigned i = 0; i < size; ++i) {
		BufferFrame frame = bm->fixPage(at(i), false);

		// write into temporary data pointer
		memcpy(temp + offset, (char *) frame.getData() + offset, pageSize);
		offset += pageSize;
	}

	// retrieve segments data

	offset = 0;
	// deserialize map size ~ amount of segments the si holds
	unordered_map<uint64_t, Segment *>::size_type mapSize;
	memcpy(&mapSize, temp + offset, sizeof(unordered_map<uint64_t, Segment *>::size_type));
	offset += sizeof(unordered_map<uint64_t, Segment *>::size_type);

	std::vector<uint64_t>::size_type extentSize = 0;

	bool fsiInitialized = false;
	// TODO: retrieve segment ID, and min max until all segments retrieved
	while (mapSize > 0) {

		FSISegment *fsi;
		// serialize segmendId
		uint64_t segId;
		memcpy(&segId, temp + offset, sizeof(uint64_t));
		offset += sizeof(uint64_t);

		// get extents vector length
		std::vector<uint64_t>::size_type extentSize;
		memcpy(&extentSize, temp + offset, sizeof(std::vector<uint64_t>::size_type));
		offset += sizeof(std::vector<uint64_t>::size_type);

		// create extents vector
		std::vector<uint64_t> extents;

		while (extentSize > 0) {

			// deserialize extents values
			uint64_t value;
			memcpy(&value, temp + offset, sizeof(uint64_t));
			offset += sizeof(uint64_t);

			extents.push_back(value);

			extentSize--;
		}

		if(!fsiInitialized){

			fsi = new FSISegment(extents, segId, bm->getPagesOnDisk() + 1);

			addToMap(make_pair(segId, (Segment *) fsi));

			fsiInitialized = true;
		} else{
			// normal segment
			Segment *seg;
			seg = new Segment(extents, segId, fsi, bm);

			addToMap(make_pair(segId, (Segment *) seg));
		}

		mapSize--;
	}

	delete temp;

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
