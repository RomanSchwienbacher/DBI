/*
 * slottedpage.cpp
 *
 *  Created on: May 22, 2013
 *      Author: roman
 */

#include <stdexcept>
#include "slottedpage.h"

SlottedPage::SlottedPage() {
	header = {0,0,0,0,0};
}

/**
 * Deletes the record pointed to by tid and updates the page header accordingly
 *
 * @param slotId: the slot ID
 */
void SlottedPage::removeRecord(uint16_t slotId) {

	if (recordsMap.count(slotId) > 0) {

		const Record* recordToDelete = recordsMap.at(slotId);
		recordsMap.erase(slotId);

		if (header.dataStart == slotId) {

			bool smallestSlotFound = false;
			uint16_t smallestSlot = 0;

			for (auto it = recordsMap.begin(); it != recordsMap.end(); ++it) {

				if (!smallestSlotFound || it->first < smallestSlot) {
					smallestSlot = it->first;
					smallestSlotFound = true;
				}
			}

			header.dataStart = smallestSlot;
		}

		if (header.firstFreeSlot > slotId) {
			header.firstFreeSlot = slotId;
		}

		header.freeSpace += recordToDelete->getLen();
		--(header.slotCount);

		delete recordToDelete;
	}
}

/**
 * Returns a pointer or reference to the readonly record associated with tid
 *
 * @param slotId: the slot ID
 *
 * @return rtrn: the record
 */
const Record* SlottedPage::lookupRecord(uint16_t slotId) {

	const Record* rtrn;

	if (recordsMap.count(slotId) > 0) {
		rtrn = recordsMap.at(slotId);
	} else {
		throw invalid_argument("No record found by given slotId");
	}

	return rtrn;
}

/**
 * Updates the record pointed to by tid with the content of record r
 *
 * @param slotId: the slot ID
 * @param r: the record
 *
 */
void SlottedPage::updateRecord(uint16_t slotId, const Record& r) {

	if (recordsMap.count(slotId) > 0) {

		const Record* recordToReplace = recordsMap.at(slotId);
		recordsMap[slotId] = &r;

		header.freeSpace += recordToReplace->getLen() - r.getLen();

		delete recordToReplace;

	} else {
		throw invalid_argument("No record found by given slotId");
	}
}

/**
 * @return the free space of the current slotted page
 */
uint64_t SlottedPage::getFreeSpace() {
	return header.freeSpace;
}

SlottedPage::~SlottedPage() {
}

