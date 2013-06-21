/*
 * scan.cpp
 *
 *  Operator class which scans a relation and produces all tuples as output
 *
 *  Created on: Jun 21, 2013
 *      Author: DRomanAvid
 */

#include <iostream>
#include "scan.h"

Scan::Scan(SchemaSegment& schemaSeg) {
	Scan::schemaSeg = &schemaSeg;
	blockSize = 0;
}

/**
 * Initializes the current scan operator by a relation r
 *
 * @param r: the relation name
 */
void Scan::open(const string &r) {

	// get relation segments for r
	spSegs = schemaSeg->getRelationSegments(r);

	if (!spSegs.empty()) {

		// set page iterator range by first segment
		pageIter = spSegs.at(0)->getPageIteratorStart();
		pageIterEnd = spSegs.at(0)->getPageIteratorEnd();

		slotIterInitialized = false;

		// set block size
		Schema::Relation rel = schemaSeg->getRelation(r);
		blockSize = rel.attributes.size();

	} else {
		cerr << "No sp-segments found for relation " << r << endl;
		throw;
	}
}

/**
 * produces the next tuple, returns false if it was the last one
 */
bool Scan::next() {

	SlottedPage* sp;
	Record* rec;

	// iterate over pages
	while (pageIter != pageIterEnd) {

		// fetch slottedPage and slot iterator range
		sp = pageIter->second;

		if (!slotIterInitialized) {
			slotIter = sp->getRecordsMap().begin();
			slotIterEnd = sp->getRecordsMap().end();
			slotIterInitialized = true;
		}

		// iterate over pages
		while (slotIter != slotIterEnd) {

			// fetch record
			rec = slotIter->second;

			// check if record is data record and not a redirection
			if (rec->isDataRecord()) {

				// TODO write registerEntries
				++slotIter;
				return true;
			}

			++slotIter;
		}

		// slotIter must be set
		slotIterInitialized = false;
		++pageIter;
	}

	return false;
}

/**
 * Returns all produced register entries
 */
vector<Register*> Scan::getOutput() {
	return registerEntries;
}

Scan::~Scan() {
}

