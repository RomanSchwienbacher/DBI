/*
 * slottedpage.cpp
 *
 *  Created on: May 22, 2013
 *      Author: roman
 */

#include <stdexcept>
#include "slottedpage.h"

SlottedPage::SlottedPage() {
	*header = {0,0,0,0};
}

/**
 * Inserts a record into slotted page
 *
 * @param r: the record
 *
 * @return slotId: slot id where r was stored
 */
uint16_t SlottedPage::insertRecord(const Record& r) {

	uint16_t slotId = createFirstFreeSlot();
	recordsMap[slotId] = &r;

	recalculateDataStart();
	header->freeSpace -= r.getLen();

	return slotId;
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

		if (header->dataStart == slotId) {
			recalculateDataStart();
		}

		header->freeSpace += recordToDelete->getLen();
		--(header->slotCount);

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

		header->freeSpace += recordToReplace->getLen() - r.getLen();

		delete recordToReplace;

	} else {
		throw invalid_argument("No record found by given slotId");
	}
}

/**
 * @return the header
 */
Header* SlottedPage::getHeader() {
	return header;
}

/**
 * @return the free space
 */
uint64_t SlottedPage::getFreeSpace() {
	return header->freeSpace;
}

/**
 * @return the first free slot (first sequential number which isn't mapped or append slot at the and)
 */
uint16_t SlottedPage::createFirstFreeSlot() {

	uint16_t rtrn = 0;

	for (auto it = recordsMap.begin(); it != recordsMap.end(); ++it) {

		if (it->first != rtrn) {
			break;
		}
		++rtrn;
	}

	(header->slotCount)++;

	return rtrn;
}

/**
 * updates header by dataStart value (calculate smallest slotId which points to a record)
 */
void SlottedPage::recalculateDataStart() {

	bool smallestSlotFound = false;
	uint16_t smallestSlot = 0;

	for (auto it = recordsMap.begin(); it != recordsMap.end(); ++it) {

		if (!smallestSlotFound || it->first < smallestSlot) {
			smallestSlot = it->first;
			smallestSlotFound = true;
		}
	}

	header->dataStart = smallestSlot;

}

/**
 * @return the serialized records map
 */
string SlottedPage::getSerializedRecordsMap() {

	// TODO go on here

	string rtrn = string("");

	for (auto it = recordsMap.begin(); it != recordsMap.end(); ++it) {

	}

	return rtrn;
}

/**
 * @return whether or not the current slotted is empty
 */
bool SlottedPage::isEmpty() {
	return header->slotCount == 0;
}

SlottedPage::~SlottedPage() {
}

