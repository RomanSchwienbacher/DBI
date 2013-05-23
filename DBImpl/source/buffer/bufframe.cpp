/*
 * bufframe.cpp
 *
 *  Created on: May 3, 2013
 *      Author: DRomanAvid
 */

#include "bufframe.h"
#include <iostream>

using namespace std;

/**
 * Constructor - sets the pageId and data pointer
 * all other fields set to initial values
 *
 * @param pageNo: page number of the file
 * @param pageData: page data pointed to by frame
 */
BufferFrame::BufferFrame(uint64_t pageNo, char* pageData) {

	pageId = pageNo;
	lsn = 0;
	state = STATE_NEW;
	whichQ = Q_FIFO;
	data = pageData;
	locker = new Lock();
	pthread_rwlock_init(&testlock, NULL);
	intent = 0;
}

/**
 * Returns the pointer to the actual data stored at the location
 * where *data points to
 */
void* BufferFrame::getData() {

	return data;
}

void BufferFrame::setData(char* pageData) {

	data = pageData;
}

/*
 * in order to invalidate data pointer set NULL
 */
void BufferFrame::invalidateData() {
	data = NULL;
}

/**
 * Returns the page id of the current frame instance
 */
uint64_t BufferFrame::getPageId() {
	return pageId;
}

/*
 * Modifies pageId
 */
void BufferFrame::setPageId(uint64_t pageNo) {
	pageId = pageNo;
}

/**
 * locks the buffer frame
 *
 * @param exclusive: if true, makes an exclusive (write) lock instead of shared (read) lock
 *
 * @return ret: 0 if lock succeeded; pthread specific error code if not
 */
int BufferFrame::lockFrame(bool exclusive) {

	int ret = 0;

	if (exclusive) {
		//ret = locker->requestWriteLock();
		ret = pthread_rwlock_wrlock(&testlock);
	} else {
		//ret = locker->requestReadLock();
		ret = pthread_rwlock_rdlock(&testlock);
	}

	return ret;
}

/**
 * tries to lock the buffer frame
 *
 * @param exclusive: if true, makes an exclusive (write) lock instead of shared (read) lock
 *
 * @return ret: 0 if lock succeeded; -1 if lock cannot be made
 */
int BufferFrame::tryLockFrame(bool exclusive) {

	int ret = -1;

	if (exclusive) {
		//ret = locker->tryWriteLock();
		ret = pthread_rwlock_trywrlock(&testlock);
	} else {
		//ret = locker->tryReadLock();
		ret = pthread_rwlock_tryrdlock(&testlock);
	}

	return ret;
}

/**
 * unlocks the latch
 *
 * @return ret: 0 if all okay, pthread error code if something went wrong
 */
int BufferFrame::unlockFrame() {

	//return locker->unlock();
	return pthread_rwlock_unlock(&testlock);
}

/**
 * sets the state of the frame
 *
 * @param st: 0 for clean, 1 for dirty, 2 for newly created (from disk)
 */

void BufferFrame::setState(int st) {

	state = st;
}

int BufferFrame::getState() {
	return state;
}

/**
 * sets the queue of the frame
 *
 * @param q: Q_FIFO or Q_LRU
 */

void BufferFrame::setWhichQ(int q) {
	whichQ = q;
}

int BufferFrame::getWhichQ() {
	return whichQ;
}

/**
 * returns the amount of waiting threads (by lock)
 */
int BufferFrame::getAmountOfWaitingThreads() {
	return locker->getWaitingThreads();
}

/**
 * Destructor of the Buffer Frame
 */
BufferFrame::~BufferFrame() {

}

