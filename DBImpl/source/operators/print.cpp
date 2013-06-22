/*
 * print.cpp
 *
 *	Operator class which prints out all input tuples in a human-readable format
 *
 *  Created on: Jun 22, 2013
 *      Author: DRomanAvid
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "print.h"

Print::Print(SchemaSegment& schemaSeg) :
		Operator(schemaSeg, OperatorType::PRINT) {

	inputOperator = NULL;
}

/**
 * Initializes the current print operator by an inputOperator and an output-stream file
 *
 * @param inputOperator: the input operator
 * @param outputFile: the filename of the output file
 */
void Print::open(Operator* inputOperator, const string& outputFile) {

	if (inputOperator != NULL) {
		Print::inputOperator = inputOperator;
		Print::outputFile = outputFile;
		outputStream.open(outputFile, ios_base::binary | ios::trunc);
	} else {
		cerr << "Input operator must be given" << endl;
		throw;
	}
}

/**
 * Fetches next tuple by input operator and writes output to stream
 */
bool Print::next() {

	bool rtrn = false;

	if (inputOperator->getType() == OperatorType::SCAN) {

		Scan* scanOperator = reinterpret_cast<Scan*>(inputOperator);
		rtrn = scanOperator->next();

		if (rtrn) {
			writeOutputToStream();
		}
	}

	return rtrn;
}

/**
 * Writes current getOuptut() result splitted by blocksize into output stream
 */
void Print::writeOutputToStream() {

	// remove content of ouptputstream by reopening it
	outputStream.close();
	outputStream.open(outputFile, ios_base::binary | ios::trunc);

	if (inputOperator->getType() == OperatorType::SCAN) {

		Scan* scanOperator = reinterpret_cast<Scan*>(inputOperator);

		stringstream output;

		// fetch relation
		Schema::Relation rel = getSchemaSeg()->getRelation(scanOperator->getRelationName());

		// write output header
		for (Schema::Relation::Attribute attr : rel.attributes) {
			output << attr.name << "\t|\t";
		}
		output << endl;

		// write output content
		unsigned i = 0;
		for (Register* reg : getOutput()) {

			if (reg->getType() == Types::Tag::Integer) {

				output << reg->getInteger() << "\t|\t";

			} else if (reg->getType() == Types::Tag::Char) {

				if (reg->getCharLength() == 1) {
					Char<1> val = reg->getChar<1>();
					output << val.toString() << "\t|\t";
				}
				else if (reg->getCharLength() == 2) {
					Char<2> val = reg->getChar<2>();
					output << val.toString() << "\t|\t";
				}
				else if (reg->getCharLength() == 20) {
					Char<20> val = reg->getChar<20>();
					output << val.toString() << "\t|\t";
				}
				else if (reg->getCharLength() == 25) {
					Char<25> val = reg->getChar<25>();
					output << val.toString() << "\t|\t";
				}
				else if (reg->getCharLength() == 50) {
					Char<50> val = reg->getChar<50>();
					output << val.toString() << "\t|\t";
				}
			}

			// check if next iteration handles new tuple, if yes: make new line
			if ((++i % scanOperator->getBlocksize()) == 0) {
				output << endl;
			}
		}

		// write string-stream to outputStream
		outputStream << output.str();
	}
}

/**
 * Reads output by input operator
 */
vector<Register*> Print::getOutput() {

	vector<Register*> rtrn(0);

	if (inputOperator->getType() == OperatorType::SCAN) {

		Scan* scanOperator = reinterpret_cast<Scan*>(inputOperator);
		rtrn = scanOperator->getOutput();
	}

	return rtrn;
}

void Print::close() {
	inputOperator = NULL;
	outputFile = "";
	outputStream.close();
}

Print::~Print() {
}

