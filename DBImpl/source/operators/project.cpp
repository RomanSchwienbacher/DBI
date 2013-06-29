/*
 * project.cpp
 *
 *	Operator class which projects to a subset of the input stream
 *
 *  Created on: Jun 24, 2013
 *      Author: DRomanAvid
 */

#include <iostream>
#include <string>
#include "project.h"

Project::Project(SchemaSegment& schemaSeg) :
		Operator(schemaSeg, OperatorType::PROJECT) {
	inputOperator = NULL;
	blockSize = 0;
}

/**
 * Initializes the current project operator by an inputOperator and a vector of attributes
 *
 * @param inputOperator: the input operator
 * @param attributes: the attributes
 */
void Project::open(Operator* inputOperator, vector<string> attributes) {

	if (inputOperator != NULL && !attributes.empty()) {
		Project::inputOperator = inputOperator;
		Project::attributes = attributes;
		blockSize = attributes.size();
	} else {
		cerr << "Input operator and attributes must be given" << endl;
		throw;
	}
}

/**
 * Fetches next tuple by input operator and projects corresponding to the attributes
 */
bool Project::next() {

	bool rtrn = inputOperator->next();

	// first clear all current register entries
	registerEntries.clear();

	// then push registers which correspond to the attributes to register entries
	for (Register* reg : inputOperator->getOutput()) {
		if (find(attributes.begin(), attributes.end(), reg->getAttrName()) != attributes.end()) {
			registerEntries.push_back(reg);
		}
	}

	return rtrn;
}

/**
 * Returns produced registerEntries
 */
vector<Register*> Project::getOutput() {
	return registerEntries;
}

void Project::close() {

	inputOperator->close();

	inputOperator = NULL;
	attributes.clear();
	registerEntries.clear();
	blockSize = 0;
}

vector<string> Project::getAttributes() {
	return attributes;
}

unsigned Project::getBlocksize() {
	return blockSize;
}

Project::~Project() {

}

