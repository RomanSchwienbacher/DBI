/*
 * tid.cpp
 *
 *	Represents a tuple-identifier, consists of a page ID and a slot ID
 *
 *  Created on: May 22, 2013
 *      Author: DRomanAvid
 */

#include "tid.h"

TID::TID(uint64_t pageId, uint16_t slotId) {
	TID::pageId = pageId;
	TID::slotId = slotId;
}

TID::~TID() {
}

