/*
 * slottedpage.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef SLOTTEDPAGE_H_
#define SLOTTEDPAGE_H_

#include "record.h"
#include <map>
#include <inttypes.h>
#include <string>

using namespace std;

typedef struct {

	uint64_t LSN;
	uint16_t slotCount;
	uint16_t dataStart;
	uint64_t freeSpace;

} Header;


class SlottedPage {

	// header for slotted page
	Header* header;
	// corresponding records map
	map<uint16_t, const Record*> recordsMap;

	uint16_t createFirstFreeSlot();
	void recalculateDataStart();

public:
	SlottedPage();

	uint16_t insertRecord(const Record& r);
	void removeRecord(uint16_t slotId);
	const Record* lookupRecord(uint16_t slotId);
	void updateRecord(uint16_t slotId, const Record& r);

	Header* getHeader();

	uint64_t getFreeSpace();

	string getSerializedRecordsMap();

	bool isEmpty();

	virtual ~SlottedPage();
};

#endif
