/*
 * sp_segment.cpp
 *
 *	Segment type SPSegment that operates on slotted pages
 *
 *	 Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#include <cstring>
#include <iostream>
#include <sstream>
#include "sp_segment.h"
#include "fsi_segment.h"
#include "../buffer/bufframe.h"
#include <stdexcept>

using namespace std;

/**
 * Constructor: initializes slotted pages
 */
SPSegment::SPSegment(vector<uint64_t> freeExtents, uint64_t segId, FSISegment *fsi,BufferManager * bm) :
		Segment(freeExtents, segId, fsi, bm) {

	for (unsigned i = 0; i < getSize(); i++) {

		SlottedPage* sp = new SlottedPage();
		sp->getHeader()->freeSpace = bm->getPageSize() - sizeof(Header);

		uint64_t pageId = at(i);

		// write slottet page to disk
		if (writeToFrame(sp, pageId)) {
			spMap.insert(make_pair(pageId, sp));
		} else {
			cerr << "Cannot write slotted page into frame" << endl;
		}
	}

	cout << "SPSegment extents: " << std::endl;
	for (unsigned i = 0; i < extents.size(); ++i) {
		std::cout << "value: " << i << ": " << extents.at(i) << std::endl;
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
TID SPSegment::insert(Record& r) {

	TID rtrn;

	SlottedPage* spHolder = NULL;

	// instantiate new record pointer by given r
	Record* newRec = new Record(r.getLen(), r.getData());

	// try to find slotted page which can hold record
	for (auto it = spMap.begin(); it != spMap.end(); ++it) {

		if (it->second->getFreeSpace() >= (newRec->getLen() + (2 * sizeof(uint16_t)) + sizeof(bool))) {

			spHolder = it->second;
			rtrn.pageId = it->first;
			break;
		}
	}

	// page to hold record found
	if (spHolder != NULL) {

		// insert record into slotted page
		rtrn.slotId = spHolder->insertRecord(*newRec);

		// write changes back to disk
		if (!writeToFrame(spHolder, rtrn.pageId)) {
			cerr << "Cannot write slotted page into frame" << endl;
		}
	}
	// no page found to hold record, so increase the segment
	else {

		// just need one more page
		vector<uint64_t> neededExtents;

		// grow by 10 pages
		vector<uint64_t> newExtents = grow(10);

		// create new slotted page
		SlottedPage* sp = new SlottedPage();
		sp->getHeader()->freeSpace = bm->getPageSize() - sizeof(Header);

		rtrn.pageId = newExtents.front();
		// insert record into slotted page
		rtrn.slotId = sp->insertRecord(*newRec);

		// write changes back to disk
		if (writeToFrame(sp, rtrn.pageId)) {
			spMap[rtrn.pageId] = sp;
		} else {
			cerr << "Cannot write slotted page into frame" << endl;
		}
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

	bool rtrn = true;

	try {

		if (tid.pageId >= 0 && tid.slotId >= 0) {

			if (spMap.count(tid.pageId) > 0) {

				SlottedPage* sp = spMap.at(tid.pageId);

				Record* recordToRemove = sp->lookupRecord(tid.slotId);

				// in case of record to remove is a redirection, first remove remote record
				if (!recordToRemove->isDataRecord()) {
					remove(recordToRemove->redirection);
				}

				sp->removeRecord(tid.slotId);

				// write changes back to disk
				if (!writeToFrame(sp, tid.pageId)) {
					cerr << "Cannot write slotted page into frame" << endl;
				}
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
Record* SPSegment::lookup(TID tid) {

	Record* rtrn;

	try {

		if (tid.pageId >= 0 && tid.slotId >= 0) {

			// read slotted page out of memory map
			if (spMap.count(tid.pageId) > 0) {

				SlottedPage* sp = spMap.at(tid.pageId);
				rtrn = sp->lookupRecord(tid.slotId);
			}
			// if not available read from frame
			else {

				// writes frame into map too
				SlottedPage* sp = readFromFrame(tid.pageId);

				if (sp != NULL) {
					rtrn = sp->lookupRecord(tid.slotId);
				} else {
					throw invalid_argument("No page found by given tid");
				}
			}

		} else {
			throw invalid_argument("Given tid is invalid");
		}

		// in case of record is a redirection, lookup for redirected data record
		if (!rtrn->isDataRecord()) {
			rtrn = lookup(rtrn->redirection);
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
bool SPSegment::update(TID tid, Record& r) {

	bool rtrn = true;

	try {

		if (tid.pageId >= 0 && tid.slotId >= 0) {

			if (spMap.count(tid.pageId) > 0) {

				// fetch slotted page
				SlottedPage* sp = spMap.at(tid.pageId);
				// slotted page which must be updated
				SlottedPage* spUpdate;

				if (sp->getRecordsMap().count(tid.slotId) > 0) {

					// fetch old record
					Record* rOld = sp->getRecordsMap().at(tid.slotId);
					// record which must be updated
					Record* rUpdate;
					// tid which must be updated
					TID tidUpdate;

					if (rOld->isDataRecord()) {
						// in case of old record is a data record, update the old record and slottedpage
						rUpdate = rOld;
						spUpdate = sp;
						tidUpdate = tid;
					} else {
						// in case of old record is a redirection, update the corresponding remote record and slotted page
						rUpdate = lookup(rOld->redirection);
						spUpdate = spMap.at(rOld->redirection.pageId);
						tidUpdate = rOld->redirection;
					}

					// calculate whether or not the sp has enough free space to contain the new record
					bool enoughFreeSpace = (spUpdate->getFreeSpace() + rUpdate->getLen()) >= r.getLen();

					// if there is enough free space just update
					if (enoughFreeSpace) {

						spUpdate->updateRecord(tidUpdate.slotId, r);

						// write changes back to disk
						if (!writeToFrame(spUpdate, tidUpdate.pageId)) {
							cerr << "Cannot write slotted page into frame" << endl;
						}
					}
					// otherwise redirect record
					else {

						// 1st step: insert record into new page with enough space
						TID redTid = insert(r);

						// 2nd step: write/update redirection into old position
						rOld->dataRecord = false;
						rOld->data = 0;
						rOld->len = 0;
						rOld->redirection = redTid;

						// 3rd step: update header information for slotted page of rOld
						sp->updateRecord(tid.slotId, *rOld);

						// write changes back to disk
						if (!writeToFrame(sp, tid.pageId)) {
							cerr << "Cannot write slotted page into frame" << endl;
						}
					}

				} else {
					throw invalid_argument("No record found by given slotId");
				}

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

/**
 * Writes a slotted page into a given buffer-frame
 *
 * @param sp: the slotted page
 * @param pageId: the page id
 *
 * @return rtrn: whether successfully or not
 */
bool SPSegment::writeToFrame(SlottedPage* sp, uint64_t pageId) {

	bool rtrn = true;

	BufferFrame frame = bm->fixPage(pageId, true);

	try {

		// 1st step: serialize
		char* spSer = sp->getSerialized();

		// 2nd step: write into frame data pointer
		memcpy(frame.getData(), spSer, bm->getPageSize());

	} catch (exception& e) {
		cerr << "An exception occurred while writing slotted page to frame: " << e.what() << endl;
		rtrn = false;
	}

	bm->unfixPage(frame, rtrn);

	return rtrn;
}

/**
 * Reads a slotted page from buffer-frame by pageId and manages sp-map
 *
 * @param pageId: the page id
 *
 * @return rtrn: SlottedPage
 */
SlottedPage* SPSegment::readFromFrame(uint64_t pageId) {

	SlottedPage* rtrn = NULL;

	BufferFrame frame = bm->fixPage(pageId, false);

	try {

		// 1st step: deserialize
		rtrn = SlottedPage::getDeserialized((char*) frame.getData());

		// 2nd step: update sp-map
		spMap[pageId] = rtrn;

	} catch (exception& e) {
		cerr << "An exception occurred while reading slotted page from frame: " << e.what() << endl;
		rtrn = NULL;
	}

	bm->unfixPage(frame, false);

	return rtrn;
}

SPSegment::~SPSegment() {
	// delete slotted pages
	for (auto it = spMap.begin(); it != spMap.end(); ++it) {
		delete (it->second);
	}
}

