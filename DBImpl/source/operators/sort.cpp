/*
 * sort.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: DRomanAvid
 */

#include "sort.h"
#include <iostream>
#include <string>

Sort::Sort(SchemaSegment& schemaSeg) : Operator::Operator(schemaSeg, OperatorType::SORT) {
	Sort::inputOperator = NULL;
}

/**
 * Initializes the current Sort Operator and launches an external sort
 *
 * @param inputOperator: the input operator
 * @param sortAttributes: the attributes to be sorted by
 */
void Sort::open(Operator* inputOperator, vector<string> sortAttributes) {

	if (inputOperator != NULL && sortAttributes.size() != 0) {
		Sort::inputOperator = inputOperator;
		Sort::sortAttributes = sortAttributes;
	} else {
		cerr << "Input operator must be given, sortAttributes vector may not be empty" << endl;
		throw;
	}

	// break the pipe and push out all registers to file

	// external sort the file

	// reload the sorted file into X registers
}

/**
 * Returns produced registerEntries
 */
vector<Register*> Sort::getOutput() {
	return registerEntries;
}

void Sort::close() {

	inputOperator->close();

	inputOperator = NULL;
	registerEntries.clear();
}

vector<string> Sort::getAttributes() {
	return inputOperator->getAttributes();
}

unsigned Sort::getBlocksize() {
	return inputOperator->getBlocksize();
}

