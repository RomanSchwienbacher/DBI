/*
 * select.cpp
 *
 *  Operator class which selects to a subset of the input stream
 *
 *  Created on: Jun 29, 2013
 *      Author: roman
 */

#include "select.h"
#include <iostream>
#include <string>

Select::Select(SchemaSegment& schemaSeg) :
		Operator(schemaSeg, OperatorType::SELECT) {
	Select::inputOperator = NULL;
}

/**
 * Initializes the current select operator by an inputOperator and a vector of predicate-attributes and values
 *
 * @param inputOperator: the input operator
 * @param predAttributes: the predicate attributes
 * @param predValues: the predicate values
 */
void Select::open(Operator* inputOperator, vector<string> predAttributes, vector<Register*> predValues) {

	if (inputOperator != NULL && predAttributes.size() == predValues.size()) {
		Select::inputOperator = inputOperator;
		Select::predAttributes = predAttributes;
		Select::predValues = predValues;
	} else {
		cerr << "Input operator must be given, predicate-attributes and values must have the same size" << endl;
		throw;
	}
}

/**
 * Fetches next tuple by input operator and selects corresponding to the predicate-attributes and values
 */
bool Select::next() {

	// first clear all current register entries
	registerEntries.clear();

	while (inputOperator->next()) {

		// represents a record
		vector<Register*> currentRecordRegisters;

		// then push registers which correspond to the attributes to register entries
		unsigned i = 0;
		for (Register* reg : inputOperator->getOutput()) {

			currentRecordRegisters.push_back(reg);

			// if one record is achieved, check given predicates
			if ((++i % inputOperator->getBlocksize()) == 0) {

				// in case of all predicates match, add currentRecordRegisters to registerEntries
				if (checkPredicatesForRecord(currentRecordRegisters)) {

					for (Register* okReg : currentRecordRegisters) {
						registerEntries.push_back(okReg);
						return true;
					}
				}

				// clear tmp record registers
				currentRecordRegisters.clear();
			}
		}
	}

	return false;
}

/**
 * checks if given record registers passes the predicates
 *
 * @param recordRegisters: the record registers
 * @return matchesPredicates: whether or not successfully
 */
bool Select::checkPredicatesForRecord(vector<Register*> recordRegisters) {

	bool matchesPredicates = true;
	unsigned attributeIndex = 0;
	vector<string>::iterator it;

	// iterate over predicate attributes
	unsigned j = 0;
	for (string predAttribute : predAttributes) {

		// check if predAttribute exists in attributes set
		if ((it = find(getAttributes().begin(), getAttributes().end(), predAttribute)) != getAttributes().end()) {

			// fetch attribute index
			attributeIndex = it - getAttributes().begin();

			// compare value stored in current record register and predicate-value
			if (!(*(recordRegisters.at(attributeIndex)) == *(predValues.at(j)))) {
				matchesPredicates = false;
				break;
			}
		} else {
			matchesPredicates = false;
			break;
		}

		++j;
	}

	return matchesPredicates;
}

/**
 * Returns produced registerEntries
 */
vector<Register*> Select::getOutput() {
	return registerEntries;
}

void Select::close() {

	inputOperator->close();

	inputOperator = NULL;
	predAttributes.clear();
	predValues.clear();
	registerEntries.clear();
}

vector<string> Select::getAttributes() {
	return inputOperator->getAttributes();
}

unsigned Select::getBlocksize() {
	return inputOperator->getBlocksize();
}

Select::~Select() {
	// TODO Auto-generated destructor stub
}

