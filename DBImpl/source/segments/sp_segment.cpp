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
#include <stdexcept>

using namespace std;

SPSegment::SPSegment(uint64_t segId) :
		Segment(segId) {
}

/**
 * Searches through the pages loking for a page with enough space to store r. If no space
 * is found, it grows the segment
 *
 * @param r: the record
 *
 * @return ret: TID identifying the location where r was stored
 */
TID SPSegment::insert(const Record& r) {
}

/**
 * Deletes the record pointed to by tid
 *
 * @param tid: the tuple ID
 *
 * @return ret: whether successfully or not
 */
bool SPSegment::remove(TID tid) {

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
		cerr << "Invalid argument: " << e.what() << endl;
		rtrn = false;
	}

	return rtrn;
}

/**
 * Returns a pointer or reference to the readonly record associated with tid
 *
 * @param tid: the tuple ID
 *
 * @return ret: the record
 */
Record* SPSegment::lookup(TID tid) {
}

/**
 * Updates the record pointed to by tid with the content of record r
 *
 * @param tid: the tuple ID
 * @param r: the record
 *
 * @return ret: whether successfully or not
 */
bool SPSegment::update(TID tid, const Record& r) {
	return false;
}

SPSegment::~SPSegment() {
}

