#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include "../../planreader/Plan.hpp"



#include "../buffer/bufman.h"
#include "../segments/segman.h"
#include "../segments/sch_segment.h"
#include "../operators/mergeJoin.h"
#include "../operators/operator.h"
#include "../operators/print.h"
#include "../operators/project.h"
#include "../operators/register.h"
#include "../operators/scan.h"
#include "../operators/select.h"
#include "../operators/sort.h"
#include "../../parsinglib/Schema.hpp"
#include "../../parsinglib/Types.hpp"

using namespace std;
using plan::Plan;

namespace testing {


static void planTest(const std::string& dbFilename, const std::string& schemaFilename, const std::string& planFilename) {

	// Set up stuff
	BufferManager bm(dbFilename, 25600ul); // bogus arguments -> 100 MB
	SegmentManager sm(1, 1, &bm);
	uint64_t spId = sm.createSegment(SegmentType::SCHEMA, 10, schemaFilename);

	SchemaSegment& seg = (SchemaSegment&) (sm.getSegment(spId));

	// Parse plan
	Plan p;
	p.fromFile(planFilename.c_str());
	p.print(std::cout);

	// buildOperatorTree and set root
	//Operator* root = buildOperatorTree(p);

}



static int launchPlantest(char** argv) {
	planTest(argv[2], argv[3], argv[4]);
	return 0;
}

}

