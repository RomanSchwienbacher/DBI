/*
 * record.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef RECORD_H_
#define RECORD_H_

#include <string.h>

class Record {

	// the size of the data
	unsigned len;

	// the data pointer of the record
	char * data;

public:

	Record& operator=(Record& rhs) = delete;
	Record(Record& t) = delete;
	Record(Record&& t);
	explicit Record(unsigned len, const char* const ptr);
	const char* getData() const;
	unsigned getLen() const;
	virtual ~Record();
};

#endif
