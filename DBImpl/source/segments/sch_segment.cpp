/*
 * sch_segment.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: DRomanAvid
 */


#include <iostream>
#include "sch_segment.h"
#include "../../parsinglib/Parser.hpp"

SchemaSegment::SchemaSegment(std::vector<uint64_t> extents, uint64_t segId, FSISegment *fsi, BufferManager *bm, const std::string &filename) :
		Segment(extents, segId, fsi, bm) {

	std::unique_ptr<Schema> schema = parseSchema(filename);

	// place data of first relation into the schemasegment
	//schema->relations.at(0).attributes..
}


/**
 * parses a file to set up a schema
 */

std::unique_ptr<Schema> SchemaSegment::parseSchema(const std::string &filename) {

	Parser p(filename);
	   try {
	      std::unique_ptr<Schema> schema = p.parse();
	      std::cout << schema->toString() << std::endl;
	   } catch (ParserError& e) {
	      std::cerr << e.what() << std::endl;
	   }

}

SchemaSegment::~SchemaSegment() {

}
