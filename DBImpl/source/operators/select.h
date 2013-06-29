/*
 * select.h
 *
 *  Created on: Jun 29, 2013
 *      Author: roman
 */

#ifndef SELECT_H_
#define SELECT_H_

#include "register.h"
#include "operator.h"
#include "../segments/sch_segment.h"
#include "../../parsinglib/Schema.hpp"
#include "../../parsinglib/Types.hpp"
#include "scan.h"
#include <vector>
#include <string>

class Select: public Operator {

	Operator* inputOperator;
	vector<string> predAttributes;
	vector<Register*> predValues;

	// register entries
	vector<Register*> registerEntries;

	bool checkPredicatesForRecord(vector<Register*> currentRecordRegisters);

public:
	Select(SchemaSegment& schemaSeg);

	void open(Operator* inputOperator, vector<string> predAttributes, vector<Register*> predValues);

	bool next();

	vector<Register*> getOutput();

	void close();

	vector<string> getAttributes();

	unsigned getBlocksize();

	virtual ~Select();
};

#endif
