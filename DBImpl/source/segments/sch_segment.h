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
#include "../segments/tid.h"

using namespace std;

class FSISegment;
class SPSegment;
class BTreeSegment;
class BufferManager;
class Schema;

class SchemaSegment: public Segment {

	// The segment manager
	SegmentManager* sm;

	// The global schema
	unique_ptr<Schema> schema;

	// Contains integer indexes for each relation
	map<string, vector<BTree<Integer, IntegerCmp>*>> intIndexMap;

	// Contains char(1) indexes for each relation
	map<string, vector<BTree<Char<1>, CharCmp<1>>*>> char1IndexMap;

	// Contains char(2) indexes for each relation
	map<string, vector<BTree<Char<2>, CharCmp<2>>*>> char2IndexMap;

	// Contains char(20) indexes for each relation
	map<string, vector<BTree<Char<20>, CharCmp<20>>*>> char20IndexMap;

	// Contains char(25) indexes for each relation
	map<string, vector<BTree<Char<25>, CharCmp<25>>*>> char25IndexMap;

	// Contains char(50) indexes for each relation
	map<string, vector<BTree<Char<50>, CharCmp<50>>*>> char50IndexMap;

	// Contains sp-segments for each relation
	map<string, vector<SPSegment*>> spsMap;

	void parseSchema(const string &filename);

	void setupIndex(Schema::Relation& r, Schema::Relation::Attribute& a);

	void setupSPSegment(Schema::Relation& r);

public:

	SchemaSegment(vector<uint64_t> extents, uint64_t segId, FSISegment * fsi, BufferManager * bm, SegmentManager* sm, const string& filename);

	void setupSchema();

	vector<Schema::Relation> getRelations();

	Schema::Relation getRelation(const string& r);

	Schema::Relation::Attribute getAttribute(const string& r, const string& a);

	vector<string> getRelationIndexTypes(const string& r);
	unsigned getRelationIndexCount(const string& r);
	vector<BTree<Integer, IntegerCmp>*> getRelationIntIndexes(const string& r);
	vector<BTree<Char<1>, CharCmp<1>>*> getRelationChar1Indexes(const string& r);
	vector<BTree<Char<2>, CharCmp<2>>*> getRelationChar2Indexes(const string& r);
	vector<BTree<Char<20>, CharCmp<20>>*> getRelationChar20Indexes(const string& r);
	vector<BTree<Char<25>, CharCmp<25>>*> getRelationChar25Indexes(const string& r);
	vector<BTree<Char<50>, CharCmp<50>>*> getRelationChar50Indexes(const string& r);

	vector<SPSegment*> getRelationSegments(const string& r);

	string getType(const string& r, const string& a);

	char* getAttributePointerByTID(TID tid, const string& r, const string& a);

	~SchemaSegment();

};

#endif /* SCH_SEGMENT_H_ */
