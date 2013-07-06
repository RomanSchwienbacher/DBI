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
	string sortAttribute;

	// filestream and Sorter for external sorting
	std::fstream fileStream;

	// register entries
	vector<Register*> registerEntries;

	vector<string> attributes;

	// type of attribute being sorted
	Types::Tag type;

	// number of data items to be sorted
	unsigned sortedSize;

	// cache size in pages to prefetch
	unsigned prefetchPages;

	// current value offset
	unsigned currentOffset;

	// current data offset
	unsigned dataOffset;

	// fetches more data
	void fetchMoreData();

public:
	Sort(SchemaSegment& schemaSeg);

	void open(Operator* inputOperator, string sortAttribute);

	bool next();

	vector<Register*> getOutput();

	void close();

	vector<string> getAttributes();

	unsigned getBlocksize();

};



#endif /* SORT_H_ */
