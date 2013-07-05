/*
 * sort.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: DRomanAvid
 */

#include "../../parsinglib/Types.hpp"
#include "../sorting/sorter.h"
#include "sort.h"
#include <iostream>
#include <string>
#include <fcntl.h>

// file descriptors used in external sorting
int fdInput = open("/tmp/extSort_db_cache", O_RDONLY);
int fdOutput = open("/tmp/extSort_db_cache", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

using namespace std;

Sort::Sort(SchemaSegment& schemaSeg) :
		Operator::Operator(schemaSeg, OperatorType::SORT) {
	Sort::inputOperator = NULL;
	prefetchPages = 1024;
	currentOffset = 0;
	dataOffset = 0;
}

/**
 * Initializes the current Sort Operator and launches an external sort
 *
 * @param inputOperator: the input operator
 * @param sortAttribute: the attribute to be sorted by
 */
void Sort::open(Operator* inputOperator, string sortAttribute) {

	Sort::inputOperator == inputOperator;
	Sort::sortAttribute == sortAttribute;

	// open file to use for external sort
	fileStream.open("/tmp/extSort_db_cache", fstream::out | fstream::trunc);

	// integers and chars with length <= 8 are sortable
	bool sortable = false;
	unsigned offset = 0;
	sortedSize = 0;

	// get Attributes from the register
	for (Register *reg : inputOperator->getOutput()) {
		if (reg->getAttrName() == sortAttribute) {

			type = reg->getType();
			// check if sortAttribute is sortable -> checked only once at the beginning
			if (!sortable && ((type == Types::Tag::Integer) || ((type == Types::Tag::Char) && (reg->getCharLength() <= 8)))) {
				sortable = true;
			}

			// break the pipe and push register to file
			fileStream.seekp(offset);
			if (type == Types::Tag::Integer) {
				int data = reg->getInteger();
				fileStream.write((char *) &data, sizeof(int));
				offset += 8;
			} else {
				// Char
				char *data = reg->getCharData();
				fileStream.write(data, 2);
				offset += 8;
			}
			sortedSize++;

		}
	}
	fileStream.flush();
	fileStream.close();

	// external sort the file
	sorting::Sorter sorter;
	sorter.externalSort(fdInput, sortedSize, fdOutput, 128 * 1024 * 1024);

	// reload the sorted file into X registers
	registerEntries.clear();

	fetchMoreData();

}

void Sort::fetchMoreData() {

	// prefetch sorted data
	char *buffer = new char[prefetchPages * _SC_PAGESIZE];

	// open file with sorted data
	fileStream.open("/tmp/extSort_db_cache", fstream::in);

	// seek to offset
	fileStream.seekg(dataOffset);

	// read the prefetched values into the buffer
	fileStream.read(buffer, prefetchPages * _SC_PAGESIZE);

	int readBytes = fileStream.gcount();

	// convert to Registers and add to vector
	if (type == Types::Tag::Integer) {
		for (unsigned i = 0; i < readBytes; i += 8) {
			int *data = (int *) (buffer + i);
			Register *reg = new Register(*data);
			registerEntries.push_back(reg);
		}
	} else {
		// Type is Char
		for (unsigned i = 0; i < readBytes; i += 8) {
			Char<2> data;
			data.data[0] = *(buffer + i);
			data.data[1] = *(buffer + i + 1);
			Register *reg = new Register(data);
			registerEntries.push_back(reg);
		}
	}

	// update data offset
	dataOffset += readBytes;

	// delete buffer
	delete[] buffer;

	fileStream.close();
}

/**
 * Returns produced registerEntries
 */
vector<Register*> Sort::getOutput() {
	return registerEntries;
}

bool Sort::next(){

	bool ret = false;

	// check if currentOffset is higher than currently held registers

	if(currentOffset >= registerEntries.size()){
		fetchMoreData();
	}

	currentOffset++;

	return ret;
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

