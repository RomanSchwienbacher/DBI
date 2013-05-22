/*
 * record.cpp
 *
 *	Represents a DB record
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#include "record.h"

Record::Record(uint64_t size, char * data) {
	Record::size = size;
	Record::data = data;
}

Record::~Record() {
}

