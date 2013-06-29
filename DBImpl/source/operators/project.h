/*
 * project.h
 *
 *  Created on: Jun 24, 2013
 *      Author: DRomanAvid
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#include "register.h"
#include "operator.h"
#include "../segments/sch_segment.h"
#include "../../parsinglib/Schema.hpp"
#include "../../parsinglib/Types.hpp"
#include "scan.h"
#include <vector>
#include <string>

using namespace std;

class Project: public Operator {

	Operator* inputOperator;
	vector<string> attributes;

	// block size of register entries
	vector<Register*> registerEntries;
	unsigned blockSize;

public:

	Project(SchemaSegment& schemaSeg);

	void open(Operator* inputOperator, vector<string> attributes);

	bool next();

	vector<Register*> getOutput();

	void close();

	vector<string> getAttributes();

	unsigned getBlocksize();

	virtual ~Project();

};

#endif
