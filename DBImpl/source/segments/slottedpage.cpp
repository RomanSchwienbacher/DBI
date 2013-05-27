/*
 * slottedpage.cpp
 *
 *  Created on: May 22, 2013
 *      Author: roman
 */

#include <stdexcept>
#include <unistd.h>
#include "slottedpage.h"

SlottedPage::SlottedPage() {
	header = (Header *) operator new(sizeof(Header));
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
uint16_t SlottedPage::getFreeSpace() {
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
 * @return whether or not the current slotted is empty
 */
bool SlottedPage::isEmpty() {
	return header->slotCount == 0;
}

/**
 * @return current records map
 */
map<uint16_t, const Record*> SlottedPage::getRecordsMap() {
	return recordsMap;
}

/**
 * @return the serialized object (header;[SlotId;RecordLength;Record;]*)
 */
char* SlottedPage::getSerialized() {

	char* rtrn = new char[sysconf(_SC_PAGESIZE)];
	int offset = 0;

	// serialize header
	memcpy(rtrn + offset, &header, sizeof(Header));
	offset += sizeof(Header);

	for (auto it = recordsMap.begin(); it != recordsMap.end(); ++it) {
		// serialize slot
		memcpy(rtrn + offset, &(it->first), sizeof(uint16_t));
		offset += sizeof(uint16_t);
		// serialize length
		memcpy(rtrn + offset, &(it->second->len), sizeof(uint16_t));
		offset += sizeof(uint16_t);
		// serialize record
		memcpy(rtrn + offset, &it->second, it->second->getLen());
		offset += it->second->getLen();
	}

	return rtrn;
}

/**
 * Factory: Parses a slotted page instance by a given pointer
 *
 * @param spPointer: the pointer
 *
 * @return rtrn: slotted page instance
 */
SlottedPage* SlottedPage::getDeserialized(char* spPointer) {

	SlottedPage* rtrn = new SlottedPage();
	int offset = 0;

	// deserialize header
	memcpy(rtrn->getHeader() + offset, spPointer, sizeof(Header));
	offset += sizeof(Header);

	// deserialize slots and records
	for (int i = 0; i < rtrn->getHeader()->slotCount; i++) {

		uint16_t slotId;
		memcpy(&slotId, spPointer + offset, sizeof(uint16_t));
		offset += sizeof(uint16_t);

		uint16_t length;
		memcpy(&length, spPointer + offset, sizeof(uint16_t));
		offset += sizeof(uint16_t);

		Record* record = new Record(length, spPointer + offset);

		rtrn->getRecordsMap().insert(make_pair(slotId, record));
	}

	return rtrn;
}

SlottedPage::~SlottedPage() {

	// delete header & records
	for (auto it = recordsMap.begin(); it != recordsMap.end(); ++it) {
		delete (it->second);
	}

	delete header;
}

