/*
 * sp_segment.cpp
 *
 *	Segment type SPSegment that operates on slotted pages
 *
 *	 Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#include "sp_segment.h"
#include "fsi_segment.h"
#include "../buffer/bufframe.h"
#include <stdexcept>

using namespace std;

SPSegment::SPSegment(vector<uint64_t> freeExtents, uint64_t segId, BufferManager * bm) : Segment(freeExtents, segId, bm) {

	for (unsigned i=0; i < getSize(); i++) {

		uint64_t pageId = at(i);
		BufferFrame frame = bm->fixPage(pageId, true);

		SlottedPage* sp = new SlottedPage();



		// free extents sind die extents die mir zur verfügung stehen

				// mit get size slottet pages initialisier
				// berechne mit sizeof header größe und materialized map größe und schreibe in header die infos

		slottedPagesMap[pageId] = sp;

	}
}

/**
 * Searches through the pages loking for a page with enough space to store r. If no space
 * is found, it grows the segment
 *
 * @param r: the record
 *
 * @return rtrn: TID identifying the location where r was stored
 */
TID SPSegment::insert(const Record& r) {

	// sofort über bm (complete slotted page) persistent schreiben (get bm page by pageId)

	TID rtrn;

	SlottedPage* spHolder = NULL;

	for (auto it = slottedPagesMap.begin(); it != slottedPagesMap.end(); ++it) {

		if (it->second->getFreeSpace() >= r.getLen()) {
			spHolder = it->second;
		}
	}

	// page found to hold record
	if (spHolder != NULL) {



	}
	// no page found to hold record, so increase the segment
	else {
		// beim grow die neue größe in pages insgesamt mitgeben

		// vom grow bekomm ich den vektor mit den neu verfügbaren extents

		// z.b. rückgabe vektor [4,6,8,9] sind sortiert und gemerged (von ungerade bis ausschließlich)

		// über buffermanager in dem fall für 4,5,8 -> 3 neue slotted pages  über buffer manager anlegen

		// records dann in erste slotted page schreiben
	}

	return rtrn;
}

/**
 * Deletes the record pointed to by tid
 *
 * @param tid: the tuple ID
 *
 * @return rtrn: whether successfully or not
 */
bool SPSegment::remove(TID tid) {

	// sofort über bm (complete slotted page) persistent schreiben (get bm page by pageId)

	bool rtrn = true;

	try {

		if (tid.pageId >= 0 && tid.slotId >= 0) {

			if (slottedPagesMap.count(tid.pageId) > 0) {

				SlottedPage* sp = slottedPagesMap.at(tid.pageId);
				sp->removeRecord(tid.slotId);
			}

		} else {
			throw invalid_argument("Given tid is invalid");
		}

	} catch (const invalid_argument& e) {
		cerr << "Invalid argument @remove segment: " << e.what() << endl;
		rtrn = false;
	}

	return rtrn;
}

/**
 * Returns a pointer or reference to the readonly record associated with tid
 *
 * @param tid: the tuple ID
 *
 * @return rtrn: the record
 */
const Record* SPSegment::lookup(TID tid) {

	const Record* rtrn;

	try {

		if (tid.pageId >= 0 && tid.slotId >= 0) {

			if (slottedPagesMap.count(tid.pageId) > 0) {

				SlottedPage* sp = slottedPagesMap.at(tid.pageId);
				rtrn = sp->lookupRecord(tid.slotId);

			} else {
				throw invalid_argument("No page found by given tid");
			}

		} else {
			throw invalid_argument("Given tid is invalid");
		}

	} catch (const invalid_argument& e) {
		cerr << "Invalid argument @lookup segment: " << e.what() << endl;
		rtrn = NULL;
	}

	return rtrn;
}

/**
 * Updates the record pointed to by tid with the content of record r
 *
 * @param tid: the tuple ID
 * @param r: the record
 *
 * @return rtrn: whether successfully or not
 */
bool SPSegment::update(TID tid, const Record& r) {

	// write persistent (complete slotted page) to db (get bm page by pageId)

	bool rtrn = true;

	try {

		if (tid.pageId >= 0 && tid.slotId >= 0) {

			if (slottedPagesMap.count(tid.pageId) > 0) {

				SlottedPage* sp = slottedPagesMap.at(tid.pageId);
				sp->updateRecord(tid.slotId, r);
			} else {
				throw invalid_argument("No page found by given tid");
			}

		} else {
			throw invalid_argument("Given tid is invalid");
		}

	} catch (const invalid_argument& e) {
		cerr << "Invalid argument @lookup segment: " << e.what() << endl;
		rtrn = false;
	}

	return rtrn;
}

SPSegment::~SPSegment() {
}

