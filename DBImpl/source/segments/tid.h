/*
 * tid.h
 *
 *	This TID uses 8 Bytes for page ID and 2 Bytes for slot ID
 *	Therefore, a slotted page consists of 2¹6 slots and we may have a maximum of 264 pages
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef TID_H_
#define TID_H_

#define __STDC_FORMAT_MACROS

#include <iostream>
#include <string>
#include <inttypes.h>

using namespace std;

typedef struct {

	// page ID (8 Bytes)
	uint64_t pageId;

	// slot ID (2 Bytes)
	uint16_t slotId;

} TID;

// hash and equal structs for hash-map key purpose

typedef struct {
	long operator()(const TID &k) const {

		char buf[128];
		sprintf(buf, "%llu%u", k.pageId, k.slotId);

		hash<string> hashFn;
		return hashFn(buf);
	}
} TIDHash;

typedef struct {
	bool operator()(const TID &x, const TID &y) const {
		return x.pageId == y.pageId && x.slotId == y.slotId;
	}
} TIDEq;

#endif
