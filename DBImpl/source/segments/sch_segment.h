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
#include <memory>
#include "../../parsinglib/Types.hpp"
#include "segment.h"

class FSISegment;
class SPSegment;
class BTreeSegment;
class BufferManager;
class Schema;

struct Attribute {
	std::string name;
	Types::Tag type;
	unsigned len;
	bool notNull;
};

class SchemaSegment: public Segment {

	// Btree index
	std::unordered_map<unsigned, BTreeSegment*> indexMap;

	// SPSegment holding relation R
	SPSegment* relR;

	// attributes
	std::vector<Attribute> attributes;

	// Primary key
	Attribute* priKey;

	std::unique_ptr<Schema> parseSchema(const std::string &filename);


public:

	SchemaSegment(std::vector<uint64_t> extents, uint64_t segId, FSISegment * fsi, BufferManager * bm, const std::string& filename);

	std::vector<std::string> getAttributeNames();
	std::vector<Types::Tag> getAttributeTypes();

	BTreeSegment& getPrimaryIndex();

	~SchemaSegment();

};

#endif /* SCH_SEGMENT_H_ */
