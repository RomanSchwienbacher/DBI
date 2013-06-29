/*
 * print.h
 *
 *  Created on: Jun 22, 2013
 *      Author: DRomanAvid
 */

#ifndef PRINT_H_
#define PRINT_H_

#include "register.h"
#include "operator.h"
#include "../segments/sch_segment.h"
#include "../../parsinglib/Schema.hpp"
#include "../../parsinglib/Types.hpp"
#include "scan.h"
#include "project.h"
#include "select.h"
#include <vector>
#include <string>

using namespace std;

class Print: public Operator {

	Operator* inputOperator;
	ofstream outputStream;
	string outputFile;

	void writeOutputToStream();

public:

	Print(SchemaSegment& schemaSeg);

	void open(Operator* inputOperator, const string& outputFile);

	bool next();

	vector<Register*> getOutput();

	void close();

	virtual ~Print();
};

#endif
