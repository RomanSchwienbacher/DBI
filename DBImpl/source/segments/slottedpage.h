/*
 * slottedpage.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef SLOTTEDPAGE_H_
#define SLOTTEDPAGE_H_

#include "record.h"
#include <unordered_map>
#include <inttypes.h>

using namespace std;

typedef struct {

	uint64_t LSN;
	uint16_t slotCount;
	uint16_t firstFreeSlot;
	uint16_t dataStart;
	uint64_t freeSpace;

} Header;


class SlottedPage {

	// header for slotted page
	Header* header;
	// corresponding records map
	unordered_map<uint16_t, const Record*> recordsMap;

public:
	SlottedPage();

	void removeRecord(uint16_t slotId);
	const Record* lookupRecord(uint16_t slotId);
	void updateRecord(uint16_t slotId, const Record& r);

	uint64_t getFreeSpace();

	virtual ~SlottedPage();
};

#endif
