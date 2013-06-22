//
//  Operator.h
//  Operator
//
//  Created by David Grotzky on 6/20/13.
//  Copyright (c) 2013 David Grotzky. All rights reserved.
//

#ifndef Operator_Operator_h
#define Operator_Operator_h

#include <vector>
#include "register.h"
#include "../segments/sch_segment.h"

using namespace std;

class Operator {

	SchemaSegment* schemaSeg;
	OperatorType type;
    
public:

	Operator(SchemaSegment& schemaSeg, OperatorType type);

	SchemaSegment* getSchemaSeg();
	OperatorType getType();

    ~Operator();
};

// Defines which operator types exist
enum class OperatorType {SCAN, PRINT};

#endif
