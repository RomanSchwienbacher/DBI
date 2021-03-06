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

bool Operator::next() {
	return false;
}

vector<Register*> Operator::getOutput() {
	return vector<Register*>(0);
}

vector<string> Operator::getAttributes() {
	return vector<string>(0);
}

unsigned Operator::getBlocksize() {
	return 0;
}

void Operator::close() {
}

Operator::~Operator() {
}

