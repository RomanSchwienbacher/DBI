/*
 * scan.cpp
 *
 *  Operator class which scans a relation and produces all tuples as output
 *
 *  Created on: Jun 21, 2013
 *      Author: DRomanAvid
 */

#include <iostream>
#include <string.h>
#include "scan.h"

Scan::Scan(SchemaSegment& schemaSeg) :
		Operator(schemaSeg, OperatorType::SCAN) {
	blockSize = 0;
	pageIterInitialized = false;
	slotIterInitialized = false;
}

/**
 * Initializes the current scan operator by a relation r
 *
 * @param r: the relation name
 */
void Scan::open(const string &r) {

	Scan::r = r;

	// get relation segments for r
	spSegs = getSchemaSeg()->getRelationSegments(r);

	if (!spSegs.empty()) {

		// set segment iterator
		segmentIter = spSegs.begin();
		segmentIterEnd = spSegs.end();

		// set block size
		Schema::Relation rel = getSchemaSeg()->getRelation(r);
		blockSize = rel.attributes.size();

	} else {
		cerr << "No sp-segments found for relation " << r << endl;
		throw;
	}
}

/**
 * produces the next tuple, returns false if it was the last one
 */
bool Scan::next() {

	SlottedPage* sp;
	Record* rec;
	TID tid;

	// iterate over segments
	while (segmentIter != segmentIterEnd) {

		if (!pageIterInitialized) {
			// set page iterator range by first segment
			pageIter = (*segmentIter)->getPageIteratorStart();
			pageIterEnd = (*segmentIter)->getPageIteratorEnd();
			pageIterInitialized = true;
		}

		// iterate over pages
		while (pageIter != pageIterEnd) {

			// fetch slottedPage and slot iterator range
			sp = pageIter->second;

			if (!slotIterInitialized) {
				slotIter = sp->getRecordsMap().begin();
				slotIterEnd = sp->getRecordsMap().end();
				slotIterInitialized = true;
			}

			// iterate over pages
			while (slotIter != slotIterEnd) {

				// generate tid
				tid.pageId = pageIter->first;
				tid.slotId = slotIter->first;

				// fetch record
				rec = slotIter->second;

				// check if record is data record and not a redirection
				if (rec->isDataRecord()) {

					// produce register entries of current record identified by tid
					produceRegisterEntries(tid);

					++slotIter;
					return true;
				}

				++slotIter;
			}

			// slotIter must be set (new page next iteration)
			slotIterInitialized = false;
			++pageIter;
		}
		// pageIter must be set (new segment next iteration)
		pageIterInitialized = false;
		++segmentIter;
	}

	return false;
}

/**
 * produces register entries by given tid
 */
void Scan::produceRegisterEntries(TID tid) {

	// generate register-entries for record
	Schema::Relation rel = getSchemaSeg()->getRelation(r);
	for (Schema::Relation::Attribute attr : rel.attributes) {

		if (attr.type == Types::Tag::Integer) {

			// read intVal by tid, record and attribute
			int intVal = 0;
			memcpy(&intVal, getSchemaSeg()->getAttributePointerByTID(tid, rel.name, attr.name), sizeof(int));

			// create register
			Register* reg = new Register(intVal);
			reg->setAttrName(attr.name);

			// push to register entries
			registerEntries.push_back(reg);

		} else if (attr.type == Types::Tag::Char) {

			if (attr.len == 1) {
				// read string-val by tid, record and attribute
				string strVal;
				memcpy(&strVal, getSchemaSeg()->getAttributePointerByTID(tid, rel.name, attr.name), attr.len);

				// initialize charVal
				Char<1> charVal;
				charVal.loadString(strVal);

				// create register
				Register* reg = new Register(charVal);
				reg->setAttrName(attr.name);

				// push to register entries
				registerEntries.push_back(reg);

			} else if (attr.len == 2) {

				string strVal;
				memcpy(&strVal, getSchemaSeg()->getAttributePointerByTID(tid, rel.name, attr.name), attr.len);
				Char<2> charVal;
				charVal.loadString(strVal);
				Register* reg = new Register(charVal);
				reg->setAttrName(attr.name);
				registerEntries.push_back(reg);

			} else if (attr.len == 20) {

				string strVal;
				memcpy(&strVal, getSchemaSeg()->getAttributePointerByTID(tid, rel.name, attr.name), attr.len);
				Char<20> charVal;
				charVal.loadString(strVal);
				Register* reg = new Register(charVal);
				reg->setAttrName(attr.name);
				registerEntries.push_back(reg);

			} else if (attr.len == 25) {

				string strVal;
				memcpy(&strVal, getSchemaSeg()->getAttributePointerByTID(tid, rel.name, attr.name), attr.len);
				Char<25> charVal;
				charVal.loadString(strVal);
				Register* reg = new Register(charVal);
				reg->setAttrName(attr.name);
				registerEntries.push_back(reg);

			} else if (attr.len == 50) {

				string strVal;
				memcpy(&strVal, getSchemaSeg()->getAttributePointerByTID(tid, rel.name, attr.name), attr.len);
				Char<50> charVal;
				charVal.loadString(strVal);
				Register* reg = new Register(charVal);
				reg->setAttrName(attr.name);
				registerEntries.push_back(reg);

			} else {
				cerr << "Character length " << attr.len << " is not supported" << endl;
			}

		} else {
			cerr << "Attribute type is not supported" << endl;
		}
	}
}

/**
 * Returns all produced register entries
 */
vector<Register*> Scan::getOutput() {
	return registerEntries;
}

vector<string> Scan::getAttributes() {
	vector<string>* rtrn = new vector<string>();
	Schema::Relation rel = getSchemaSeg()->getRelation(r);

	for (Schema::Relation::Attribute attr : rel.attributes) {
		rtrn->push_back(attr.name);
	}

	return *rtrn;
}

/**
 * Closes current operator by resetting class-variables
 */
void Scan::close() {
	r = "";
	blockSize = 0;
	registerEntries.clear();
	spSegs.clear();
	segmentIter = segmentIterEnd;
	pageIter = pageIterEnd;
	slotIter = slotIterEnd;
	pageIterInitialized = false;
	slotIterInitialized = false;
}

string Scan::getRelationName() {
	return r;
}

unsigned Scan::getBlocksize() {
	return blockSize;
}

Scan::~Scan() {
	for (Register* reg : registerEntries) {
		delete reg;
	}
}

