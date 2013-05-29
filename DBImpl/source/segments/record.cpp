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

Record::Record(Record&& t) : len(t.len), data(t.data), dataRecord(t.dataRecord) {
	t.dataRecord = true;
	t.len = 0;
	t.data = 0;
}

Record::Record(uint16_t len, const char* ptr) : len(len), dataRecord(true) {
	data = static_cast<char*>(malloc(len));
	memcpy(data, ptr, len);
}

char* Record::getData() {
	return data;
}

uint16_t Record::getLen() {
	return len;
}

bool Record::isDataRecord() {
	return dataRecord;
}

Record::~Record() {
	if (isDataRecord()) {
		free(data);
	}
}

