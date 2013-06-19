#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "../buffer/bufman.h"
#include "../segments/segman.h"
#include "../segments/sch_segment.h"
#include "../../parsinglib/Schema.hpp"
#include "../../parsinglib/Types.hpp"

using namespace std;

namespace testing {

static void schemaTest(const std::string& dbFilename, const std::string& schemaFilename) {

	/*
	// Set up stuff
	BufferManager bm(dbFilename, 25600ul); // bogus arguments -> 100 MB
	SegmentManager sm(1, 1, &bm);
	uint64_t spId = sm.createSegment(SegmentType::SCHEMA, 10, schemaFilename);

	SchemaSegment& seg = (SchemaSegment&) (sm.getSegment(spId));

	cout << "Check parsed relations" << endl;

	// check amount of parsed relations
	assert(seg.getRelations().size() == 3);

	// iterate over all relations
	for (Relation r : seg.getRelations()) {
		if (r.name == "employee") {

			cout << "Check employee" << endl;

			// check indexes
			assert(seg.getRelationIndexes(r.name).size() == 2);
			for (BTree index : seg.getRelationIndexes(r.name)) {
				assert(index.getName() == "id" || index.getName() == "country_id");
			}

			// check segments
			assert(seg.getSegments(r.name).size() > 0);

			// check types
			assert(seg.getAttributeType(r.name, "id") == Types::Tag::Integer);
			assert(seg.getAttributeType(r.name, "country_id") == Types::Tag::Char);
			assert(seg.getAttributeLength(r.name, "country_id") == 2);
			assert(seg.getAttributeType(r.name, "mgr_id") == Types::Tag::Integer);
			assert(seg.getAttributeType(r.name, "salery") == Types::Tag::Integer);
			assert(seg.getAttributeType(r.name, "first_name") == Types::Tag::Char);
			assert(seg.getAttributeLength(r.name, "first_name") == 20);
			assert(seg.getAttributeType(r.name, "middle") == Types::Tag::Char);
			assert(seg.getAttributeLength(r.name, "middle") == 1);
			assert(seg.getAttributeType(r.name, "last_name") == Types::Tag::Char);
			assert(seg.getAttributeLength(r.name, "last_name") == 20);

		} else if (r.name == "country") {

			cout << "Check country" << endl;

			// check indexes
			assert(seg.getRelationIndexes(r.name).size() == 1);
			for (BTree index : seg.getRelationIndexes(r.name)) {
				assert(index.getName() == "country_id");
			}

			// check segments
			assert(seg.getSegments(r.name).size() > 0);

			// check types
			assert(seg.getAttributeType(r.name, "country_id") == Types::Tag::Char);
			assert(seg.getAttributeLength(r.name, "country_id") == 2);
			assert(seg.getAttributeType(r.name, "short_name") == Types::Tag::Char);
			assert(seg.getAttributeLength(r.name, "short_name") == 20);
			assert(seg.getAttributeType(r.name, "long_name") == Types::Tag::Char);
			assert(seg.getAttributeLength(r.name, "long_name") == 50);

		} else if (r.name == "department") {

			cout << "Check department" << endl;

			// check indexes
			assert(seg.getRelationIndexes(r.name).size() == 1);
			for (BTree index : seg.getRelationIndexes(r.name)) {
				assert(index.getName() == "id");
			}

			// check segments
			assert(seg.getSegments(r.name).size() > 0);

			// check types
			assert(seg.getAttributeType(r.name, "id") == Types::Tag::Integer);
			assert(seg.getAttributeType(r.name, "name") == Types::Tag::Char);
			assert(seg.getAttributeLength(r.name, "name") == 25);
			assert(seg.getAttributeType(r.name, "country_id") == Types::Tag::Char);
			assert(seg.getAttributeLength(r.name, "country_id") == 2);

		} else {
			cerr << "Parsed unknown relation: " << r.name << endl;
			assert(false);
		}
	}
	*/

}

static int launchSchematest(char** argv) {
	schemaTest(argv[2], argv[3]);
	return 0;
}

}

