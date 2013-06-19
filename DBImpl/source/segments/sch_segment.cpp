/*
 * sch_segment.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: DRomanAvid
 */

#include <iostream>
#include "sch_segment.h"
#include "../../parsinglib/Parser.hpp"
#include "record.h"

SchemaSegment::SchemaSegment(vector<uint64_t> extents, uint64_t segId, FSISegment *fsi, BufferManager *bm, SegmentManager* sm, const string &filename) :
		Segment(extents, segId, fsi, bm) {

	SchemaSegment::sm = sm;

	// parse schema by given filename
	parseSchema(filename);
}

/**
 * performs schema setup (indexes, slotted-pages)
 */
void SchemaSegment::setupSchema() {

	// iterate over all parsed relations
	for (Schema::Relation r : getRelations()) {

		// setup primary key indexes
		for (unsigned pk : r.primaryKey) {
			setupIndex(r, r.attributes.at(pk));
		}

		// setup sp segment
		setupSPSegment(r);

	}
}

/**
 * Returns all relations of current schema
 */
vector<Schema::Relation> SchemaSegment::getRelations() {
	return schema->relations;
}

/**
 * Returns relation instance by relation name
 *
 * @param r: the relation
 *
 */
Schema::Relation SchemaSegment::getRelation(const string& r) {

	Schema::Relation rtrn("");
	bool found = false;

	for (Schema::Relation rel : getRelations()) {
		if (rel.name == r) {
			rtrn = rel;
			found = true;
			break;
		}
	}

	if (!found) {
		cerr << "cannot find relation " << r << endl;
	}

	return rtrn;
}

/**
 * Returns attribute instance by relation and attribute name
 *
 * @param r: the relation
 * @param r: the attribute
 *
 */
Schema::Relation::Attribute SchemaSegment::getAttribute(const string& r, const string& a) {

	Schema::Relation::Attribute rtrn = Schema::Relation::Attribute();
	bool found = false;

	Schema::Relation rel = getRelation(r);

	for (Schema::Relation::Attribute attr : rel.attributes) {
		if (attr.name == a) {
			rtrn = attr;
			found = true;
			break;
		}
	}

	if (!found) {
		cerr << "cannot find attribute " << a << " in relation" << r << endl;
	}

	return rtrn;
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
 * Returns the sp-segments of a given relation r
 *
 * @param r: the relation
 */
vector<SPSegment*> SchemaSegment::getRelationSegments(const string& r) {

	vector<SPSegment*> rtrn(0);

	try {
		rtrn = spsMap.at(r);
	} catch (out_of_range& oor) {
		cerr << "No segments for relation " << r << " found: " << oor.what() << endl;
	}

	return rtrn;
}

/**
 * Returns the type of a given attribute
 *
 * @param r: the relation
 * @param a: the attribute
 */
string SchemaSegment::getType(const string& r, const string& a) {

	const Schema::Relation::Attribute attr = getAttribute(r, a);

	string rtrn = "";

	Types::Tag type = attr.type;
	switch (type) {
	case Types::Tag::Integer: {
		rtrn = "Integer";
		break;
	}
	case Types::Tag::Char: {
		std::stringstream ss;
		ss << "Char(" << attr.len << ")";
		rtrn = ss.str();
		break;
	}
	}

	return rtrn;
}

/**
 * Returns the pointer of a given attribute by tid
 *
 * @param tid: the tuple-identifier
 * @param r: the relation
 * @param a: the attribute
 */
char* SchemaSegment::getAttributePointerByTID(TID tid, const string& r, const string& a) {

	char* rtrn = NULL;
	unsigned offset = 0;
	bool found = false;

	// fetch relation
	Schema::Relation rel = getRelation(r);

	Record* rec;

	// iterate over all sp-segments for given relation
	for (SPSegment* sps : getRelationSegments(r)) {

		// if record was found: process
		if ((rec = sps->lookup(tid)) != NULL) {

			rtrn = rec->data;

			// iterate over attributes to calculate offset
			for (Schema::Relation::Attribute attr : rel.attributes) {

				// if attribute was found, add offset to record pointer
				if (attr.name == a) {
					rtrn += offset;
					found = true;
					break;
				}
				// otherwise increase offset
				else {
					if (attr.type == Types::Tag::Integer) {
						offset += sizeof(int);
					} else if (attr.type == Types::Tag::Integer) {
						offset += (sizeof(char) * attr.len);
					}
				}
			}

			break;
		}
	}

	return found ? rtrn : NULL;
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
void SchemaSegment::setupIndex(Schema::Relation& r, Schema::Relation::Attribute& a) {

	// create new index
	uint64_t spId = sm->createSegment(SegmentType::BTREE, 10, "");
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

void SchemaSegment::setupSPSegment(Schema::Relation& r) {

	// create new sp-segment for relation
	uint64_t spId = sm->createSegment(SegmentType::SLOTTED_PAGE, 10, "");
	SPSegment& sp = (SPSegment&) (sm->getSegment(spId));

	// add segment to existing entry
	if (spsMap.count(r.name) > 0) {
		spsMap.at(r.name).push_back(&sp);
	}
	// or create new one
	else {
		vector<SPSegment*>* v = new vector<SPSegment*>();
		v->push_back(&sp);
		spsMap.insert(make_pair(r.name, *v));
	}

}

SchemaSegment::~SchemaSegment() {

}
