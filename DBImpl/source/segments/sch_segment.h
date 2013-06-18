/*
 * sch_segment.h
 *
 *  Created on: Jun 18, 2013
 *      Author: dave
 */

#ifndef SCH_SEGMENT_H_
#define SCH_SEGMENT_H_

#include <vector>
#include <string>

class FSISegment;
class BufferManager;

class SchemaSegment : Segment {

	// Btree index
	std::unordered_map<unsigned int, BTreeSegment*> indexMap;

	// SPSegment holding relation R
	SPSegment* relR;

	// attributes
	std::vector<std::string> attributeNames;
	std::vector<Types::Tag>;

public:

	SchemaSegment(std::vector<uint64_t> extents, uint64_t segId, FSISegment *fsi, BufferManager * bm, std::string &schema);

	std::vector<std::string> getAttributeNames();
	std::vector<Types::Tag> getAttributeTypes();

	BTreeSegment& getIndex();

	~SchemaSegment();

};


#endif /* SCH_SEGMENT_H_ */
