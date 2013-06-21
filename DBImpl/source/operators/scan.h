/*
 * scan.h
 *
 *  Created on: Jun 21, 2013
 *      Author: DRomanAvid
 */

#ifndef SCAN_H_
#define SCAN_H_

#include "register.h"
#include "../segments/sch_segment.h"
#include "../segments/sp_segment.h"
#include "../segments/slottedpage.h"
#include "../../parsinglib/Schema.hpp"
#include <vector>
#include <string>
#include <iterator>

using namespace std;

class Scan {

	// block size of register entries
	unsigned blockSize;

	vector<Register*> registerEntries;

	map<uint64_t, SlottedPage*>::iterator pageIter;
	map<uint64_t, SlottedPage*>::iterator pageIterEnd;

	// controls whether slotIter gets set or not
	bool slotIterInitialized;

	map<uint16_t, Record*>::iterator slotIter;
	map<uint16_t, Record*>::iterator slotIterEnd;

	SchemaSegment* schemaSeg;
	vector<SPSegment*> spSegs;

public:

	Scan(SchemaSegment& schemaSeg);

	void open(const string &r);

	bool next();

	vector<Register*> getOutput();

	void close();

	virtual ~Scan();

};

#endif
