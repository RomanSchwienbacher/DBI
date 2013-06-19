/*
 * sch_segment.h
 *
 *  Created on: Jun 18, 2013
 *      Author: dave
 */

#ifndef SCH_SEGMENT_H_
#define SCH_SEGMENT_H_

#include <map>
#include <vector>
#include <string>
#include <memory>
#include "../../parsinglib/Types.hpp"
#include "../../parsinglib/Schema.hpp"
#include "segment.h"
#include "segman.h"
#include "btree_segment.h"
#include "../index/btree.h"
#include "sp_segment.h"

using namespace std;

class FSISegment;
class SPSegment;
class BTreeSegment;
class BufferManager;
class Schema;

class SchemaSegment: public Segment {

	/*
	// Btree index
	unordered_map<unsigned, BTreeSegment*> indexMap;

	// SPSegment holding relation R
	SPSegment* relR;

	// attributes
	vector<Attribute> attributes;

	// Primary key
	Attribute* priKey;
	*/

	// The segment manager
	SegmentManager* sm;

	// The global schema
	unique_ptr<Schema> schema;

	// Contains indexes for each relation
	map<string, vector<BTreeSegment*>> indexMap;

	// Contains sp-segments for each relation
	map<string, vector<SPSegment*>> spsMap;

	void parseSchema(const string &filename);

	void setupIndex(Schema::Relation& r, Schema::Relation::Attribute& a);

	void setupSPSegment(Schema::Relation& r);

public:

	SchemaSegment(vector<uint64_t> extents, uint64_t segId, FSISegment * fsi, BufferManager * bm, SegmentManager* sm, const string& filename);

	vector<Schema::Relation> getRelations();

	Schema::Relation getRelation(const string& r);

	Schema::Relation::Attribute getAttribute(const string& r, const string& a);

	vector<BTreeSegment*> getRelationIndexes(const string& r);

	vector<SPSegment*> getRelationSegments(const string& r);

	string getType(const string& r, const string& a);

	/*
	vector<string> getAttributeNames();
	vector<Types::Tag> getAttributeTypes();

	BTreeSegment& getPrimaryIndex();
	*/

	~SchemaSegment();

};

#endif /* SCH_SEGMENT_H_ */
