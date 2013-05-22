/*
 * record.cpp
 *
 *	Represents a DB record
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#include "record.h"
#include <stdlib.h>

using namespace std;

Record::Record(Record&& t) : len(t.len), data(t.data) {
	t.data = 0;
	t.len = 0;
}

Record::Record(unsigned len, const char* const ptr) : len(len) {
	data = static_cast<char*>(malloc(len));
	memcpy(data, ptr, len);
}

const char* Record::getData() const {
	return data;
}

unsigned Record::getLen() const {
	return len;
}

Record::~Record() {
	free(data);
}

