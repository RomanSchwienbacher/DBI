/*
 * sch_segment.cpp
 *
 *	Segment type SchemaSegment that operates on Metadata/Schema information
 *
 *	 Created on: June 18, 2013
 *      Author: DRomanAvid
 */

#include <cstring>
#include <iostream>
#include <sstream>
#include "sch_segment.h"
#include "fsi_segment.h"
#include "../buffer/bufframe.h"
#include <stdexcept>

using namespace std;

/**
 * Constructor:
 */
SchemaSegment::SchemaSegment(vector<uint64_t> freeExtents, uint64_t segId, FSISegment * fsi, BufferManager * bm, const std::string& schema) :
		Segment(freeExtents, segId, fsi, bm) {

	// TODO implement
}

vector<std::string> SchemaSegment::getAttributeNames() {
	// TODO implement
	return attributeNames;
}

vector<Types::Tag> SchemaSegment::getAttributeTypes() {
	// TODO implement
	return attributeTypes;
}

BTreeSegment& SchemaSegment::getIndex() {
	// TODO implement
}

SchemaSegment::~SchemaSegment() {

}

