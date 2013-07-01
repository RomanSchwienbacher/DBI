/*
 * sorter.h
 *
 *	Purpose: Provides sorting methods
 *
 *  @author David Grotzky
 *  @author Roman Schwienbacher
 */

#ifndef SORTER_H_
#define SORTER_H_

#include <iostream>
#include "../operators/register.h"
#include "../../parsinglib/Types.hpp"
using namespace std;

namespace sorting {

class Sorter {
public:
	void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize, Types::Tag dataType, unsigned charLen);

private:
	int createAndSortChunks(int fdInput, uint64_t size, uint64_t memSize, Types::Tag dataType, unsigned charLen, int blockSize);
	int mergeChunks(int chunkNum, uint64_t memSize, uint64_t size, int fdOutput, int blockSize);
};

} /* namespace sorting */
#endif /* SORTER_H_ */
