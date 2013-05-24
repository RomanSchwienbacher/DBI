/*
 * slottedpage.cpp
 *
 *  Created on: May 22, 2013
 *      Author: roman
 */

#include "slottedpage.h"

SlottedPage::SlottedPage() {

}

/**
 * Deletes the record pointed to by tid and updates the page header accordingly
 *
 * @param tid: the tuple ID
 *
 * @return ret: whether successfully or not
 */
void SlottedPage::removeRecord(uint16_t slotId) {

	if (recordsMap.count(slotId) > 0) {

		Record* recordToDelete = recordsMap.at(slotId);
		recordsMap.erase(slotId);

		if (header->dataStart == slotId) {

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

		if (header->firstFreeSlot > slotId) {
			header->firstFreeSlot = slotId;
		}

		header->freeSpace += recordToDelete->getLen();
		--(header->slotCount);

		delete recordToDelete;
	}
}

SlottedPage::~SlottedPage() {
}

