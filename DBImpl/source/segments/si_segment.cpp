/*
 * si_segment.cpp
 *
 *  Created on: May 20, 2013
 *      Author: dave
 */

#include "si_segment.h"

SISegment::SISegment(uint64_t maxPageId){

	segId = currentId++;
	fsiSeg = new FSISegment(1, currentId++, maxPageId);

}


uint64_t SISegment::createSegment(uint64_t size){

	fsiSeg->getFreePages(size);

}
