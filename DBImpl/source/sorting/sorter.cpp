/*
 * sorter.cpp
 *
 *	Purpose: Provides sorting methods
 *
 *  @author David Grotzky
 *  @author Roman Schwienbacher
 */

#include "sorter.h"
#include <iostream>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <math.h>
#include <list>
#include <sstream>
#include <string.h>
#include <algorithm>

using namespace std;

namespace sorting {

/*
 * calculates amount of chunks, creates chunks and sorts them internal
 *
 * @param fdInput the file descriptor of the input file
 * @param size amount of 64 bit unsigned integer values
 * @param memSize bytes of main memory
 * @return amount of generated and sorted chunks
 */
int Sorter::createAndSortChunks(int fdInput, uint64_t size, uint64_t memSize) {

	// calculate needed chunks
	int chunks = (int) ceil((double) (size * 8) / memSize);

	// iterate over chunks
	for (int i = 0; i < chunks; ++i) {

		// char-buffer pointer with size of given memSize
		char* charBufferPointer = new char[memSize];

		// read file content into buffer
		int readBytes = read(fdInput, charBufferPointer, memSize);

		if (readBytes < 0) {
			cerr << "cannot read file " << strerror(errno) << endl;
			return -1;
		}

		// cast char buffer into uint64_t buffer
		uint64_t* uintBufferPointer = (uint64_t*) charBufferPointer;

		// perform chunk sort
		sort(uintBufferPointer, uintBufferPointer + (readBytes / 8));

		// create filename
		char tmpFileName[128];
		sprintf(tmpFileName, "/tmp/tmp_output%d.txt", i);

		// create tmp chunk output file descriptor
		int tmpFdOutput = open(tmpFileName, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

		if (tmpFdOutput < 0) {
			cerr << "cannot open file " << tmpFileName << strerror(errno) << endl;
			return -1;
		}

		// write content
		if (write(tmpFdOutput, uintBufferPointer, readBytes) < 0) {
			cerr << "cannot write file " << strerror(errno) << endl;
			return -1;
		}

		// close tmp descriptor
		if (close(tmpFdOutput) < 0) {
			cerr << "cannot write file " << strerror(errno) << endl;
			return -1;
		}
	}

	return chunks;
}

/**
 * mergeChunks takes internally sorted chunks and merges them in the
 * correct order
 *
 * @param chunkNum the amount of chunks to be merged
 * @param memSize the amount of memory used for merging
 * @param size maximum amount of values to be sorted
 * @param fdOutput file descriptor for output of merged values
 */

int Sorter::mergeChunks(int chunks, uint64_t memSize, uint64_t size, int fdOutput) {

	// calculate page size (in bytes)
	const int pageSize = sysconf(_SC_PAGESIZE);

	// size of one partition (in bytes) in memory
	uint partSize = memSize / chunks;

	// find out page size and how many pages fit in a partition of memory
	uint pagesPerPart = partSize / pageSize;

	// increase memSize to minimum amount required to fit one page per chunk
	if (pagesPerPart < 1) {
		memSize = chunks * pageSize;
		partSize = memSize / chunks;
		pagesPerPart = partSize / pageSize;
		cerr << "memSize too small to load one page per chunk. Increased to " << memSize << " bytes." << endl;
	}

	// allocate memory to load chunks into
	uint64_t *memPtr = (uint64_t *) operator new(memSize);

	/*
	 * partition memory - need one partition pointer per partition
	 * also initialize partition offsets
	 * */
	uint64_t *partPtr[chunks];
	uint partOffset[chunks];
	partPtr[0] = memPtr;
	partOffset[0] = 0;
	for (int i = 1; i < chunks; ++i) {
		partPtr[i] = partPtr[i - 1] + (partSize / sizeof(uint64_t));
		partOffset[i] = 0;
	}

	// handles one file handler per chunk file
	int chunkFd[chunks];

	// stores filenames
	string filenames[chunks];

	// finished
	short finished[chunks];

	for (int i = 0; i < chunks; ++i) {

		// open file handles to all chunks, initialize file offsets and finished
		char filename[128];

		//TODO: find way to convert 1,2,3,... to 001, 002, 003,...
		sprintf(filename, "/tmp/tmp_output%d.txt", i);

		// try to open each file and add the file descriptor to the fds array
		if ((chunkFd[i] = open(filename, O_RDONLY)) < 0) {
			cerr << "cannot open file '" << filename << "': " << strerror(errno) << endl;
			return -1;
		}

		// store filename
		filenames[i] = filename;

		// set finished to false
		finished[i] = 0;
	}

	// read chunk from disk to buffer
	uint bytesRead[chunks];
	uint valuesRead[chunks];
	for (int i = 0; i < chunks; ++i) {
		bytesRead[i] = read(chunkFd[i], partPtr[i], pagesPerPart * pageSize);
		valuesRead[i] = bytesRead[i] / sizeof(uint64_t);
	}

	/*
	 * initialize the smallest value to the first value
	 * initialize the chunk with the smallest value to the first chunk
	 * number of finished chunks is zero at the beginning
	 */
	uint64_t *smallestValuePtr = memPtr;
	uint chunkWithSmallest = 0;
	int finishedChunks = 0;

	while (finishedChunks < chunks) {

		/*
		 * go through each partition comparing the numbers of each chunk
		 * at position offset while keeping a pointer to the smallest value
		 * and the index of the chunk holding the smallest value
		 */
		for (int i = 0; i < chunks; ++i) {
			if (finished[i])
				continue;
			if ((*(partPtr[i] + partOffset[i]) < *smallestValuePtr)) {
				smallestValuePtr = partPtr[i] + partOffset[i];
				chunkWithSmallest = i;
			}

		}

		/*
		 * After going through all partitions write out the smallest value
		 */

		if (write(fdOutput, smallestValuePtr, sizeof(uint64_t)) < 0) {
			cerr << "Error writing output file: " << strerror(errno) << endl;
			return -1;
		}

		//cout << "Value " << *smallestValuePtr << " written from chunk " << chunkWithSmallest << endl;

		//*smallestValuePtr = UINT64_MAX;

		/*
		 * decrease amount of values to still be sorted (given as parameter size)
		 * and break if amount reached
		 */
		if (--size <= 0) {
			cout << "end of values reached." << endl;
			break;
		}

		/*
		 * increase offset of chunk that holds the current smallest value
		 * and check if partition has ended
		 */
		if (++partOffset[chunkWithSmallest] >= valuesRead[chunkWithSmallest]) {
			// end of values within partition reached, load new values
			bytesRead[chunkWithSmallest] = read(chunkFd[chunkWithSmallest], partPtr[chunkWithSmallest], pagesPerPart * pageSize);

			if (bytesRead[chunkWithSmallest] < 0) {
				cerr << "cannot read file " << strerror(errno) << endl;
				return -1;
			}

			// reset partition offset to the beginning
			partOffset[chunkWithSmallest] = 0;

			if (bytesRead[chunkWithSmallest] == 0) {
				// no more data to be read from this chunk - set to finished
				cout << "No more bytes to be read from chunk " << chunkWithSmallest << endl;
				finished[chunkWithSmallest] = 1;
				++finishedChunks;
			} else {
				cout << bytesRead[chunkWithSmallest] << " bytes read from chunk: " << chunkWithSmallest << endl;
				valuesRead[chunkWithSmallest] = bytesRead[chunkWithSmallest] / sizeof(uint64_t);

			}

		}

		// reset smallestValuePtr
		for (int i = 0; i < chunks; ++i) {
			if (!finished[i]) {
				smallestValuePtr = partPtr[i] + partOffset[i];
				chunkWithSmallest = i;
				break;
			}
		}

	}

	// remove temporary chunk files
	for (int i = 0; i < chunks; i++) {

		char * buffer = new char[filenames[i].length()];
		strcpy(buffer,filenames[i].c_str());

		if (remove(buffer) != 0) {
			cerr << "Error deleting tmp chunk file " << filenames[i] << endl;
		}
	}

	// free up chunk memory
	//operator delete(memPtr);

	return 0;
}

/*
 * sorts 64 bit unsigned integer values stored in the file given by the fdInput file descriptor
 *
 * @param fdInput the file descriptor of the input file
 * @param size amount of 64 bit unsigned integer values
 * @param fdOutput the file descriptor of the output file
 * @param memSize bytes of main memory
 */
void Sorter::externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {

	try {

		if (fdInput == -1) {
			throw invalid_argument("Cannot open input file");
		}

		if (size == 0) {
			throw invalid_argument("Size cannot be 0");
		}

		if (fdOutput == -1) {
			throw invalid_argument("Cannot open output file");
		}

		if (memSize == 0 || memSize % 8 != 0) {
			throw invalid_argument("Memory size cannot be 0 and must be a multiple of 8");
		}

		// calculate amount of chunks, create chunks and sort them internal
		int chunks = createAndSortChunks(fdInput, size, memSize);

		if (chunks >= 0) {
			cout << "Amount of generated chunks: " << chunks << endl;

			// combine chunks by mergeing them in the correct order
			mergeChunks(chunks, memSize, size, fdOutput);

		} else {
			cerr << "Error while creating and sorting chunks" << endl;
		}

	} catch (const invalid_argument& e) {
		cerr << "Invalid argument: " << e.what() << endl;
	}

	close(fdInput);
	close(fdOutput);
}

}

/*
 int main(int argc, char** argv) {

 if(argc != 3){

 cerr << "Usage: sort <input file> <output file> <memory size>" << endl;
 return -1;
 }

 int fdInput;

 if((fdInput = open(argv[0], O_RDONLY)) < 0){
 cerr << "cannot open file '" << argv[0] << "': " << strerror(errno) << endl;

 }

 int fdOutput;

 if((fdOutput = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR)) < 0){
 cerr << "cannot open file '" << argv[1] << "': " << strerror(errno) << endl;

 }

 uint64_t memSize = atol(argv[2]);

 sorting::externalSort(fdInput, 20, fdOutput, memSize);

 return 0;
 }
 */

