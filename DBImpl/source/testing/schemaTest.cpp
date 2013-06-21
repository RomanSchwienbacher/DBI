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

	// Set up stuff
	BufferManager bm(dbFilename, 25600ul); // bogus arguments -> 100 MB
	SegmentManager sm(1, 1, &bm);
	uint64_t spId = sm.createSegment(SegmentType::SCHEMA, 10, schemaFilename);

	SchemaSegment& seg = (SchemaSegment&) (sm.getSegment(spId));

	cout << "Check parsed relations" << endl;

	// check amount of parsed relations
	assert(seg.getRelations().size() == 3);

	// iterate over all relations
	for (Schema::Relation r : seg.getRelations()) {
		if (r.name == "employee") {

			cout << "Check employee" << endl;

			// check indexes
			assert(seg.getRelationIndexCount(r.name) == 2);
			for (string type : seg.getRelationIndexTypes(r.name)) {
				assert(type == "Integer" || type == "Char(2)");
			}
			for (BTree<Integer, IntegerCmp>* index : seg.getRelationIntIndexes(r.name)) {
				assert(index->getSegment()->getName() == "id");
			}
			for (BTree<Char<2>, CharCmp<2>>* index : seg.getRelationChar2Indexes(r.name)) {
				assert(index->getSegment()->getName() == "country_id");
			}

			// check segments
			assert(seg.getRelationSegments(r.name).size() > 0);

			// check types
			assert(seg.getType(r.name, "id") == "Integer");
			assert(seg.getType(r.name, "country_id") == "Char(2)");
			assert(seg.getType(r.name, "mgr_id") == "Integer");
			assert(seg.getType(r.name, "salery") == "Integer");
			assert(seg.getType(r.name, "first_name") == "Char(20)");
			assert(seg.getType(r.name, "middle") == "Char(1)");
			assert(seg.getType(r.name, "last_name") == "Char(20)");

		} else if (r.name == "country") {

			cout << "Check country" << endl;

			// check indexes
			assert(seg.getRelationIndexCount(r.name) == 1);
			for (string type : seg.getRelationIndexTypes(r.name)) {
				assert(type == "Char(2)");
			}
			for (BTree<Char<2>, CharCmp<2>>* index : seg.getRelationChar2Indexes(r.name)) {
				assert(index->getSegment()->getName() == "country_id");
			}

			// check segments
			assert(seg.getRelationSegments(r.name).size() > 0);

			// check types
			assert(seg.getType(r.name, "country_id") == "Char(2)");
			assert(seg.getType(r.name, "short_name") == "Char(20)");
			assert(seg.getType(r.name, "long_name") == "Char(50)");

		} else if (r.name == "department") {

			cout << "Check department" << endl;

			// check indexes
			assert(seg.getRelationIndexCount(r.name) == 1);
			for (string type : seg.getRelationIndexTypes(r.name)) {
				assert(type == "Integer");
			}
			for (BTree<Integer, IntegerCmp>* index : seg.getRelationIntIndexes(r.name)) {
				assert(index->getSegment()->getName() == "id");
			}

			// check segments
			assert(seg.getRelationSegments(r.name).size() > 0);

			// check types
			assert(seg.getType(r.name, "id") == "Integer");
			assert(seg.getType(r.name, "name") == "Char(25)");
			assert(seg.getType(r.name, "country_id") == "Char(2)");

		} else {
			cerr << "Parsed unknown relation: " << r.name << endl;
			assert(false);
		}
	}
}

static int launchSchematest(char** argv) {
	schemaTest(argv[2], argv[3]);
	return 0;
}

}

