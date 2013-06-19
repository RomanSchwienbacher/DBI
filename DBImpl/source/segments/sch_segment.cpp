/*
 * sch_segment.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: DRomanAvid
 */

#include <iostream>
#include "sch_segment.h"
#include "../../parsinglib/Parser.hpp"

SchemaSegment::SchemaSegment(vector<uint64_t> extents, uint64_t segId, FSISegment *fsi, BufferManager *bm, const string &filename) :
		Segment(extents, segId, fsi, bm) {

	// parse schema by given filename
	parseSchema(filename);

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
 * parses a file to set up a schema
 */
void SchemaSegment::parseSchema(const string &filename) {

	Parser p(filename);
	try {
		schema = p.parse();
		cout << schema->toString() << endl;
	} catch (ParserError& e) {
		cerr << e.what() << endl;
	}
}

void SchemaSegment::setupIndex(BTreeSegment *index) {

}

SchemaSegment::~SchemaSegment() {

}
