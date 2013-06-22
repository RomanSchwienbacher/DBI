/*
 * operator.cpp
 *
 *  Super-Class for physical operators
 *
 *  Created on: Jun 22, 2013
 *      Author: DRomanAvid
 */

#include "operator.h"

Operator::Operator(SchemaSegment& schemaSeg, OperatorType type) {
	Operator::schemaSeg = &schemaSeg;
	Operator::type = type;
}

/**
 * Returns current schema segment
 */
SchemaSegment* Operator::getSchemaSeg() {
	return schemaSeg;
}

/**
 * Returns operator type
 */
OperatorType Operator::getType() {
	return type;
}

Operator::~Operator() {
}

