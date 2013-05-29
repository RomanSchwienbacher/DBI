/*
 * record.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef RECORD_H_
#define RECORD_H_

#include <string.h>
#include <inttypes.h>
#include "tid.h"

class Record {

public:

	bool dataRecord;

	// the data pointer of the record
	char * data;

	// the size of the data
	uint16_t len;

	// the redirection (if it is not a data record)
	TID redirection;

	Record& operator=(Record& rhs) = delete;
	Record(Record& t) = delete;
	Record(Record&& t);
	Record(uint16_t len, const char* ptr);
	char* getData();
	uint16_t getLen();
	bool isDataRecord();
	void setDataRecord(bool dataRecord);
	virtual ~Record();
};

#endif
