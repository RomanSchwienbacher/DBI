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

using namespace std;

// TODO initialize parent segment properly
SPSegment::SPSegment() : Segment(0) {
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
 * Deletes the record pointed to by tid and updates the page header accordingly
 *
 * @param tid: the tuple ID
 *
 * @return ret: whether successfully or not
 */
bool SPSegment::remove(TID tid) {
	return false;
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

