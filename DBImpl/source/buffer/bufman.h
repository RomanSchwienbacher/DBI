#ifndef BUFMAN_H
#define	BUFMAN_H

#include <string>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <list>
#include <mutex>
#include "bufframe.h"

using namespace std;

class BufferManager {
	int pageSize;

	// file stream to source file on disk managed by the buffer manager
	fstream fsSource;

	// size of the buffer manager in number of frames managed
	uint64_t buffSize;

	// amount of pages on disk
	uint64_t pagesOnDisk;

	// buffer frames map used for page management
	unordered_map<uint64_t, BufferFrame*> bufferFramesMap;

	// linked list for fifo queue
	list<uint64_t> fifoQ;

	// double linked list OR second chance for lru queue; use struct{pageId, unfixedBit}
	list<uint64_t> lruQ;

	/* memory pointer: this pointer points to the beginning of virtual memory needed
	 * for the actual page data read from disk file and pointed to by buffer frames
	 */
	char *bufferedData;

	BufferFrame& reclaimFrame();

	void writeBackToDisk(BufferFrame& frame);

public:
	BufferManager(const std::string& filename, uint64_t size);

	BufferFrame& fixPage(uint64_t pageId, bool exclusive);

	void unfixPage(BufferFrame& frame, bool isDirty);

	uint64_t getPagesOnDisk();

	~BufferManager();

};

#endif
