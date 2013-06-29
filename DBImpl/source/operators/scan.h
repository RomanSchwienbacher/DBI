/*
 * scan.h
 *
 *  Created on: Jun 21, 2013
 *      Author: DRomanAvid
 */

#ifndef SCAN_H_
#define SCAN_H_

#include "register.h"
#include "operator.h"
#include "../segments/sch_segment.h"
#include "../segments/sp_segment.h"
#include "../segments/slottedpage.h"
#include "../segments/tid.h"
#include "../../parsinglib/Schema.hpp"
#include "../../parsinglib/Types.hpp"
#include <vector>
#include <string>
#include <iterator>

using namespace std;

class Scan: public Operator {

	vector<SPSegment*> spSegs;

	// relation name
	string r;

	// block size of register entries
	vector<Register*> registerEntries;
	unsigned blockSize;

	vector<SPSegment*>::iterator segmentIter;
	vector<SPSegment*>::iterator segmentIterEnd;

	// controls whether pageIter gets set or not
	bool pageIterInitialized;
	map<uint64_t, SlottedPage*>::iterator pageIter;
	map<uint64_t, SlottedPage*>::iterator pageIterEnd;

	// controls whether slotIter gets set or not
	bool slotIterInitialized;
	map<uint16_t, Record*>::iterator slotIter;
	map<uint16_t, Record*>::iterator slotIterEnd;

	void produceRegisterEntries(TID tid);

public:

	Scan(SchemaSegment& schemaSeg);

	void open(const string &r);

	bool next();

	vector<Register*> getOutput();

	void close();

	string getRelationName();

	unsigned getBlocksize();

	vector<string> getAttributes();

	virtual ~Scan();

};

#endif
