#ifndef BUFFRAME_H
#define BUFFRAME_H

#include <pthread.h>
#include <stdint.h>
#include "locker.h"

class BufferFrame {

	// pageId is used by callers to identify which buffer frame they want to have
	uint64_t pageId;

	// line sequence number of last change, incrementing numbers?
	uint64_t lsn;

	// lock instance to perform read/write locks on frame
	Lock* locker;

	// clean == 0 / dirty == 1 / newly created == 2 etc.
	int state;

	// last queue indicates which queue the frame was in previously
	int whichQ;

	// *data points to the buffered page in virtual memory
	char *data;

	pthread_rwlock_t testlock;

public:

	// intent to lock
	int intent;

	static const int STATE_CLEAN = 0;
	static const int STATE_DIRTY = 1;
	static const int STATE_NEW = 2;

	static const int Q_FIFO = 0;
	static const int Q_LRU = 1;

	BufferFrame(uint64_t pageNo, char* pageData);

	void* getData();
	void setData(char* pageData);
	void invalidateData();

	uint64_t getPageId();
	void setPageId(uint64_t pageNo);

	void setState(int st);
	int getState();

	void setWhichQ(int q);
	int getWhichQ();

	int lockFrame(bool exclusive);
	int tryLockFrame(bool exclusive);
	int unlockFrame();

	int getAmountOfWaitingThreads();

	~BufferFrame();
};

#endif
