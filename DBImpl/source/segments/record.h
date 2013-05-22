/*
 * record.h
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef RECORD_H_
#define RECORD_H_

#include <stdint.h>

class Record {

	// the size of the data
	uint64_t size;

	// the data pointer of the record
	char * data;

public:
	Record(uint64_t size, char * data);
	virtual ~Record();
};

#endif
