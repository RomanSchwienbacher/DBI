/*
 * sort.h
 *
 *  Created on: Jul 1, 2013
 *      Author: DRomanAvid
 */

#ifndef SORT_H_
#define SORT_H_

#include "register.h"
#include "operator.h"
#include "../segments/sch_segment.h"
#include "../../parsinglib/Schema.hpp"
#include "../../parsinglib/Types.hpp"
#include "scan.h"
#include <vector>
#include <string>

class Sort: public Operator {

	Operator* inputOperator;
	vector<string> sortAttributes;

	// register entries
	vector<Register*> registerEntries;

public:
	Sort(SchemaSegment& schemaSeg);

	void open(Operator* inputOperator, vector<string> sortAttributes);

	bool next();

	vector<Register*> getOutput();

	void close();

	vector<string> getAttributes();

	unsigned getBlocksize();

};



#endif /* SORT_H_ */
