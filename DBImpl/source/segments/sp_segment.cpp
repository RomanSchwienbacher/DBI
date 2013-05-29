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
SPSegment::SPSegment(vector<uint64_t> freeExtents, uint64_t segId, BufferManager * bm) :
		Segment(freeExtents, segId, bm) {

	for (unsigned i = 0; i < getSize(); i++) {

		SlottedPage* sp = new SlottedPage();
		sp->getHeader()->freeSpace = bm->getPageSize() - sizeof(Header);

		uint64_t pageId = at(i);

		// write slottet page to disk
		if (writeToFrame(sp, pageId)) {
			spMap.insert(make_pair(pageId, sp));
		} else {
			cerr << "Cannot write frame into slotted page" << endl;
		}
	}

	// DEBUG
	std::cout << "SPSegment extents: " << std::endl;
	for (int i = 0; i < extents.size(); ++i) {
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
TID SPSegment::insert(const Record& r) {

	TID rtrn;

	SlottedPage* spHolder = NULL;

	// instantiate new record pointer by given r
	Record* newRec = new Record(r.getLen(), r.getData());

	// try to find slotted page which can hold record
	for (auto it = spMap.begin(); it != spMap.end(); ++it) {

		if (it->second->getFreeSpace() >= (newRec->getLen() + (2 * sizeof(uint16_t)))) {

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
			cerr << "Cannot write frame into slotted page" << endl;
		}
	}
	// no page found to hold record, so increase the segment
	else {

		// just need one more page
		vector<uint64_t> neededExtents;

		// TODO: get correct free extents
		neededExtents.push_back(extents.back());
		neededExtents.push_back(extents.back() + 1);

		// grow by needed extents
		vector<uint64_t> newExtents = grow(neededExtents);

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
			cerr << "Cannot write frame into slotted page" << endl;
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
				sp->removeRecord(tid.slotId);

				// write changes back to disk
				if (!writeToFrame(sp, tid.pageId)) {
					cerr << "Cannot write frame into slotted page" << endl;
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
const Record* SPSegment::lookup(TID tid) {

	const Record* rtrn;

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

	bool rtrn = true;

	try {

		if (tid.pageId >= 0 && tid.slotId >= 0) {

			if (spMap.count(tid.pageId) > 0) {

				// fetch slotted page
				SlottedPage* sp = spMap.at(tid.pageId);
				if (sp->getRecordsMap().count(tid.slotId) > 0) {

					// fetch old record
					const Record* rOld = sp->getRecordsMap().at(tid.slotId);

					// calculate whether or not the sp has enough free space to contain the new record
					bool enoughFreeSpace = (sp->getFreeSpace() + rOld->getLen()) >= r.getLen();

					// if there is enough free space just update
					if (enoughFreeSpace) {

						sp->updateRecord(tid.slotId, r);

						// write changes back to disk
						if (!writeToFrame(sp, tid.pageId)) {
							cerr << "Cannot write frame into slotted page" << endl;
						}
					}
					// otherwise reallocate record
					else {
						cout << "REALLOCATE BABY!!" << endl;
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

