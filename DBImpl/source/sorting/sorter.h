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
using namespace std;

namespace sorting {

class Sorter {
public:
	void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize);

private:
	int createAndSortChunks(int fdInput, uint64_t size, uint64_t memSize);
	int mergeChunks(int chunkNum, uint64_t memSize, uint64_t size, int fdOutput);
};

} /* namespace sorting */
#endif /* SORTER_H_ */
