/*
 * sch_segment.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: DRomanAvid
 */

#include <iostream>
#include "sch_segment.h"
#include "../../parsinglib/Parser.hpp"

SchemaSegment::SchemaSegment(vector<uint64_t> extents, uint64_t segId, FSISegment *fsi, BufferManager *bm, SegmentManager* sm, const string &filename) :
		Segment(extents, segId, fsi, bm) {

	SchemaSegment::sm = sm;

	// parse schema by given filename
	parseSchema(filename);

	// iterate over all parsed relations
	for (Schema::Relation r : getRelations()) {

		// setup primary key indexes
		for (unsigned pk : r.primaryKey) {
			setupIndex(r, r.attributes.at(pk));
		}
	}

	// place data of first relation into the schemasegment

	//schema->relations.at(0).attributes..
}

/**
 * Returns all relations of current schema
 */
vector<Schema::Relation> SchemaSegment::getRelations() {
	return schema->relations;
}

/**
 * Returns the indexes of a given relation r
 *
 * @param r: the relation
 */
vector<BTreeSegment*> SchemaSegment::getRelationIndexes(const string& r) {

	vector<BTreeSegment*> rtrn(0);

	try {
		rtrn = indexMap.at(r);
	} catch (out_of_range& oor) {
		cerr << "No index for relation " << r << " found: " << oor.what() << endl;
	}

	return rtrn;
}

/**
 * Parses a file to set up a schema
 *
 * @param filename: the filename
 */
void SchemaSegment::parseSchema(const string& filename) {

	Parser p(filename);
	try {
		schema = p.parse();
		cout << schema->toString() << endl;
	} catch (ParserError& e) {
		cerr << e.what() << endl;
	}
}

/**
 * Setup index for a given relation r on an attribute a
 *
 * @param r: the relation
 * @param a: the attribute
 */
void SchemaSegment::setupIndex(Schema::Relation r, Schema::Relation::Attribute a) {

	// create new index
	uint64_t spId = sm->createSegment(SegmentType::BTREE, 10, NULL);
	BTreeSegment& seg = (BTreeSegment&) (sm->getSegment(spId));
	seg.setName(a.name);

	// add index to existing entry
	if (indexMap.count(r.name) > 0) {
		indexMap.at(r.name).push_back(&seg);
	}
	// or create new one
	else {
		vector<BTreeSegment*>* v = new vector<BTreeSegment*>();
		v->push_back(&seg);
		indexMap.insert(make_pair(r.name, *v));
	}
}

SchemaSegment::~SchemaSegment() {

}
