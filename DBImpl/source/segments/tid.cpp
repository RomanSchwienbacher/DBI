/*
 * tid.cpp
 *
 *	This TID uses 8 Bytes for page ID and 2 Bytes for slot ID
 *	Therefore, a slotted page consists of 2¹⁶ slots and we may have a maximum of 2⁶⁴ pages
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef TID_H_
#define TID_H_
#define __STDC_FORMAT_MACROS

#include <iostream>
#include <stdint.h>
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

		// FACKEN UND KICKEN
		// JETZ ABA LOS!

		char hashChar[128];
		sprintf(hashChar, "%" PRIu64 "%" PRIu16, &(k.pageId), &(k.slotId));

		hash<string> hashFn;
		return hashFn(hashChar);
	}
} TIDHash;

typedef struct {
	bool operator()(const TID &x, const TID &y) const {
		return x.pageId == y.pageId && x.slotId == y.slotId;
	}
} TIDEq;

#endif
