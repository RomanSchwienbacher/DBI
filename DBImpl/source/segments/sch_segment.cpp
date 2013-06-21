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
 * Returns the amount of indexes of a given relation r
 * @param r: the relation
 */
unsigned SchemaSegment::getRelationIndexCount(const string& r) {

	unsigned rtrn = 0;

	if (!intIndexMap.empty() && intIndexMap.count(r) > 0) {
		rtrn += intIndexMap.at(r).size();
	}
	if (!char1IndexMap.empty() && char1IndexMap.count(r) > 0) {
		rtrn += char1IndexMap.at(r).size();
	}
	if (!char2IndexMap.empty() && char2IndexMap.count(r) > 0) {
		rtrn += char2IndexMap.at(r).size();
	}
	if (!char20IndexMap.empty() && char20IndexMap.count(r) > 0) {
		rtrn += char20IndexMap.at(r).size();
	}
	if (!char25IndexMap.empty() && char25IndexMap.count(r) > 0) {
		rtrn += char25IndexMap.at(r).size();
	}
	if (!char50IndexMap.empty() && char50IndexMap.count(r) > 0) {
		rtrn += char50IndexMap.at(r).size();
	}

	return rtrn;
}

/**
 * Returns the used index types of a given relation r
 * @param r: the relation
 */
vector<string> SchemaSegment::getRelationIndexTypes(const string& r) {

	vector<string> rtrn;

	if (!intIndexMap.empty() && intIndexMap.count(r) > 0) {
		rtrn.push_back("Integer");
	}
	if (!char1IndexMap.empty() && char1IndexMap.count(r) > 0) {
		rtrn.push_back("Char(1)");
	}
	if (!char2IndexMap.empty() && char2IndexMap.count(r) > 0) {
		rtrn.push_back("Char(2)");
	}
	if (!char20IndexMap.empty() && char20IndexMap.count(r) > 0) {
		rtrn.push_back("Char(20)");
	}
	if (!char25IndexMap.empty() && char25IndexMap.count(r) > 0) {
		rtrn.push_back("Char(25)");
	}
	if (!char50IndexMap.empty() && char50IndexMap.count(r) > 0) {
		rtrn.push_back("Char(50)");
	}

	return rtrn;
}

/**
 * Returns the integer indexes of a given relation r
 * @param r: the relation
 */
vector<BTree<Integer, IntegerCmp>*> SchemaSegment::getRelationIntIndexes(const string& r) {

	vector<BTree<Integer, IntegerCmp>*> rtrn(0);
	try {
		rtrn = intIndexMap.at(r);
	} catch (out_of_range& oor) {
		cerr << "No integer index for relation " << r << " found: " << oor.what() << endl;
	}
	return rtrn;
}

/**
 * Returns the char(1) indexes of a given relation r
 * @param r: the relation
 */
vector<BTree<Char<1>, CharCmp<1>>*> SchemaSegment::getRelationChar1Indexes(const string& r) {

	vector<BTree<Char<1>, CharCmp<1>>*> rtrn(0);
	try {
		rtrn = char1IndexMap.at(r);
	} catch (out_of_range& oor) {
		cerr << "No char(1) index for relation " << r << " found: " << oor.what() << endl;
	}
	return rtrn;
}

/**
 * Returns the char(2) indexes of a given relation r
 * @param r: the relation
 */
vector<BTree<Char<2>, CharCmp<2>>*> SchemaSegment::getRelationChar2Indexes(const string& r) {

	vector<BTree<Char<2>, CharCmp<2>>*> rtrn(0);
	try {
		rtrn = char2IndexMap.at(r);
	} catch (out_of_range& oor) {
		cerr << "No char(2) index for relation " << r << " found: " << oor.what() << endl;
	}
	return rtrn;
}

/**
 * Returns the char(20) indexes of a given relation r
 * @param r: the relation
 */
vector<BTree<Char<20>, CharCmp<20>>*> SchemaSegment::getRelationChar20Indexes(const string& r) {

	vector<BTree<Char<20>, CharCmp<20>>*> rtrn(0);
	try {
		rtrn = char20IndexMap.at(r);
	} catch (out_of_range& oor) {
		cerr << "No char(20) index for relation " << r << " found: " << oor.what() << endl;
	}
	return rtrn;
}

/**
 * Returns the char(25) indexes of a given relation r
 * @param r: the relation
 */
vector<BTree<Char<25>, CharCmp<25>>*> SchemaSegment::getRelationChar25Indexes(const string& r) {

	vector<BTree<Char<25>, CharCmp<25>>*> rtrn(0);
	try {
		rtrn = char25IndexMap.at(r);
	} catch (out_of_range& oor) {
		cerr << "No char(25) index for relation " << r << " found: " << oor.what() << endl;
	}
	return rtrn;
}

/**
 * Returns the char(50) indexes of a given relation r
 * @param r: the relation
 */
vector<BTree<Char<50>, CharCmp<50>>*> SchemaSegment::getRelationChar50Indexes(const string& r) {

	vector<BTree<Char<50>, CharCmp<50>>*> rtrn(0);
	try {
		rtrn = char50IndexMap.at(r);
	} catch (out_of_range& oor) {
		cerr << "No char(50) index for relation " << r << " found: " << oor.what() << endl;
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
					} else if (attr.type == Types::Tag::Char) {
						offset += (sizeof(char) * attr.len);
					} else {
						cerr << "Given type is not supported" << endl;
						throw;
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
 * supported attribute-types: Integer, Char(1), Char(2), Char(20), Char(25), Char(50)
 *
 * @param r: the relation
 * @param a: the attribute
 */
void SchemaSegment::setupIndex(Schema::Relation& r, Schema::Relation::Attribute& a) {

	// create new btree segment
	uint64_t spId = sm->createSegment(SegmentType::BTREE, 10, "");
	BTreeSegment& seg = (BTreeSegment&) (sm->getSegment(spId));
	seg.setName(a.name);

	// create new btree
	if (a.type == Types::Tag::Integer) {

		BTree<Integer, IntegerCmp>* integerIndex = new BTree<Integer, IntegerCmp>(seg);

		// add index to existing entry
		if (intIndexMap.count(r.name) > 0) {
			intIndexMap.at(r.name).push_back(integerIndex);
		}
		// or create new one
		else {
			vector<BTree<Integer, IntegerCmp>*>* v = new vector<BTree<Integer, IntegerCmp>*>();
			v->push_back(integerIndex);
			intIndexMap.insert(make_pair(r.name, *v));
		}

	} else if (a.type == Types::Tag::Char && a.len == 1) {

		BTree<Char<1>, CharCmp<1>>* charIndex = new BTree<Char<1>, CharCmp<1>>(seg);

		if (char1IndexMap.count(r.name) > 0) {
			char1IndexMap.at(r.name).push_back(charIndex);
		} else {
			vector<BTree<Char<1>, CharCmp<1>>*>* v = new vector<BTree<Char<1>, CharCmp<1>>*>();
			v->push_back(charIndex);
			char1IndexMap.insert(make_pair(r.name, *v));
		}

	} else if (a.type == Types::Tag::Char && a.len == 2) {

		BTree<Char<2>, CharCmp<2>>* charIndex = new BTree<Char<2>, CharCmp<2>>(seg);

		if (char2IndexMap.count(r.name) > 0) {
			char2IndexMap.at(r.name).push_back(charIndex);
		} else {
			vector<BTree<Char<2>, CharCmp<2>>*>* v = new vector<BTree<Char<2>, CharCmp<2>>*>();
			v->push_back(charIndex);
			char2IndexMap.insert(make_pair(r.name, *v));
		}

	} else if (a.type == Types::Tag::Char && a.len == 20) {

		BTree<Char<20>, CharCmp<20>>* charIndex = new BTree<Char<20>, CharCmp<20>>(seg);

		if (char20IndexMap.count(r.name) > 0) {
			char20IndexMap.at(r.name).push_back(charIndex);
		} else {
			vector<BTree<Char<20>, CharCmp<20>>*>* v = new vector<BTree<Char<20>, CharCmp<20>>*>();
			v->push_back(charIndex);
			char20IndexMap.insert(make_pair(r.name, *v));
		}

	} else if (a.type == Types::Tag::Char && a.len == 25) {

		BTree<Char<25>, CharCmp<25>>* charIndex = new BTree<Char<25>, CharCmp<25>>(seg);

		if (char25IndexMap.count(r.name) > 0) {
			char25IndexMap.at(r.name).push_back(charIndex);
		} else {
			vector<BTree<Char<25>, CharCmp<25>>*>* v = new vector<BTree<Char<25>, CharCmp<25>>*>();
			v->push_back(charIndex);
			char25IndexMap.insert(make_pair(r.name, *v));
		}

	} else if (a.type == Types::Tag::Char && a.len == 50) {

		BTree<Char<50>, CharCmp<50>>* charIndex = new BTree<Char<50>, CharCmp<50>>(seg);

		if (char50IndexMap.count(r.name) > 0) {
			char50IndexMap.at(r.name).push_back(charIndex);
		} else {
			vector<BTree<Char<50>, CharCmp<50>>*>* v = new vector<BTree<Char<50>, CharCmp<50>>*>();
			v->push_back(charIndex);
			char50IndexMap.insert(make_pair(r.name, *v));
		}

	} else {
		cerr << "Given type is not supported" << endl;
		throw;
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

	// delete integer index vectors
	for (auto it = intIndexMap.begin(); it != intIndexMap.end(); ++it) {
		delete &(it->second);
	}

	// delete char(1) index vectors
	for (auto it = char1IndexMap.begin(); it != char1IndexMap.end(); ++it) {
		delete &(it->second);
	}

	// delete char(2) index vectors
	for (auto it = char2IndexMap.begin(); it != char2IndexMap.end(); ++it) {
		delete &(it->second);
	}

	// delete char(20) index vectors
	for (auto it = char20IndexMap.begin(); it != char20IndexMap.end(); ++it) {
		delete &(it->second);
	}

	// delete char(25) index vectors
	for (auto it = char25IndexMap.begin(); it != char25IndexMap.end(); ++it) {
		delete &(it->second);
	}

	// delete char(50) index vectors
	for (auto it = char50IndexMap.begin(); it != char50IndexMap.end(); ++it) {
		delete &(it->second);
	}

	// delete slotted page vectors
	for (auto it = spsMap.begin(); it != spsMap.end(); ++it) {
		delete &(it->second);
	}
}
