/*
 * tid.h
 *
 *	This TID uses 8 Bytes for page ID and 2 Bytes for slot ID
 *	Therefore, a slotted page consists of 2¹⁶ slots and we may have a maximum of 2⁶⁴ pages
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#ifndef TID_H_
#define TID_H_

#include <stdint.h>

class TID {

	// page ID (8 Bytes)
	uint64_t pageId;

	// slot ID (2 Bytes)
	uint16_t slotId;


public:
	TID(uint64_t pageId, uint16_t slotId);
	virtual ~TID();
};

#endif
