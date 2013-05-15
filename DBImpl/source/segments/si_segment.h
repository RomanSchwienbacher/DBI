/*
 * si_segment.h
 *
 *  Created on: May 15, 2013
 *      Author: dave
 */

#ifndef SI_SEGMENT_H_
#define SI_SEGMENT_H_

class SISegment : private Segment{

	Segment& createSegment(uint64_t size);
	int dropSegment(uint64_t segId);
	Segment& growSegment(uint64_t segId, uint64_t newSize);
	Segment& retrieveSegment(uint64_t segId);

public:

	SISegment(uint64_t size);

};



#endif /* SI_SEGMENT_H_ */
