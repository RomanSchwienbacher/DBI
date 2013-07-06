/*
 * mergeJoin.h
 *
 *  Created on: Jul 6, 2013
 *      Author: roman
 */

#ifndef MERGEJOIN_H_
#define MERGEJOIN_H_

#include "register.h"
#include "operator.h"
#include "../segments/sch_segment.h"
#include "../../parsinglib/Schema.hpp"
#include "../../parsinglib/Types.hpp"
#include "scan.h"
#include <vector>
#include <string>

using namespace std;

// Defines functionalities
enum class Functionality {
	ONE_ONE, ONE_N, N_ONE, N_M
};

// Defines N:M match states
enum class N_M_MatchState {
	OFF, ACTIVE, BROKEN
};

class MergeJoin: public Operator {

	Operator* inputOperatorLeft;
	Operator* inputOperatorRight;
	string attributeLeft;
	string attributeRight;
	vector<string> attributes;

	Register* oldLeft;
	Register* oldRight;
	Register* curLeft;
	Register* curRight;
	bool fetchNextLeft;
	bool preventFetchingNext;

	N_M_MatchState n_mMatchState;

	Functionality functionality;

	// block size of register entries
	vector<Register*> registerEntries;
	unsigned blockSize;

	void reCalculateFunctionality();

public:

	MergeJoin(SchemaSegment& schemaSeg);

	void open(Operator* inputOperatorLeft, Operator* inputOperatorRight, string attributeLeft, string attributeRight);

	bool next();

	vector<Register*> getOutput();

	void close();

	vector<string> getAttributes();

	unsigned getBlocksize();

	virtual ~MergeJoin();

};

#endif
