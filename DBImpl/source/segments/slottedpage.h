/*
 * slottedpage.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef SLOTTEDPAGE_H_
#define SLOTTEDPAGE_H_

#include "record.h"
#include <inttypes.h>

using namespace std;

typedef struct {

	uint64_t LSN;
	uint16_t slotCount;
	uint16_t firstFreeSlot;
	uint16_t dataStart;
	uint64_t freeSpace;

} Header;

typedef struct {
	uint16_t offset;
	uint16_t length;
} Slot;

class SlottedPage {

	// header for slotted page
	Header* header;
	// corresponding slots
	vector<Slot*> slots;
	// corresponding records
	vector<Record*> records;


public:
	SlottedPage();



	virtual ~SlottedPage();
};

#endif
