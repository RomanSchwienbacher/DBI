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

class Record {

	// the data pointer of the record
	char * data;

public:

	// the size of the data
	uint16_t len;

	Record& operator=(Record& rhs) = delete;
	Record(Record& t) = delete;
	Record(Record&& t);
	explicit Record(uint16_t len, const char* const ptr);
	const char* getData() const;
	uint16_t getLen() const;
	virtual ~Record();
};

#endif
