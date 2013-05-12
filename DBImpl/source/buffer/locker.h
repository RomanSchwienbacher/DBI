/*
 * locker.h
 *
 *  Created on: May 4, 2013
 *      Author: DRomanAvid
 */

#ifndef LOCK_H_
#define LOCK_H_

#include <condition_variable>
#include <mutex>

using namespace std;

class Lock {

	// determines if current lock is write locked
	bool writeLocked;
	// amount of read locks
	unsigned readLocks;
	// amount of waiting threads
	unsigned waitingThreads;
	// mutual exclusion
	mutex* m;
	// condition variable for m
	condition_variable* condVar;

public:
	Lock();

	int requestReadLock();
	int requestWriteLock();
	int unlock();
	int getWaitingThreads();

	virtual ~Lock();
};

#endif
