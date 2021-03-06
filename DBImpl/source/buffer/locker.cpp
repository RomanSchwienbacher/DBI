/*
 * locker.cpp
 *
 *  Created on: May 4, 2013
 *      Author: roman
 */

#include "locker.h"
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>

using namespace std;


/**
 * Constructor - initializes attributes
 */
Lock::Lock() {
	writeLocked = false;
	readLocks = 0;
	waitingThreads = 0;
	m = new mutex;
	condVar = new condition_variable;
}

/*
 * invokes a read-lock request
 */
int Lock::requestReadLock() {

	unique_lock<mutex> readLock(*m);

	++waitingThreads;

	// in case of write lock is active, wait
	while (writeLocked) {
		condVar->wait(readLock);
	}

	--waitingThreads;

	// increase read-locks
	++readLocks;
	writeLocked = false;

	return 0;
}

/*
 * invokes a non-blocking read-lock request
 */
int Lock::tryReadLock() {

	int ret;
	unique_lock<mutex> readLock(*m);

	if(writeLocked){
		readLock.unlock();
		ret = -1;
	} else {

		// in case of write lock is active, wait
		while (writeLocked) {
			condVar->wait(readLock);
		}

		--waitingThreads;

		// increase read-locks
		++readLocks;
		writeLocked = false;
		ret = 0;

	}

	return ret;

}

/*
 * invokes a write-lock request
 */
int Lock::requestWriteLock() {

	unique_lock<mutex> writeLock(*m);

	++waitingThreads;

	// in case of active readLocks or one write lock wait
	while (readLocks > 0 || writeLocked) {
		condVar->wait(writeLock);
	}

	--waitingThreads;

	writeLocked = true;

	return 0;
}

/*
 * invokes a non-blocking write-lock request
 */
int Lock::tryWriteLock() {

	unique_lock<mutex> writeLock(*m);

	int ret;

	if((readLocks > 0 || writeLocked)){
		ret = -1;
	} else {
		++waitingThreads;

		// in case of active readLocks or one write lock wait
		while (readLocks > 0 || writeLocked) {
			condVar->wait(writeLock);
		}

		--waitingThreads;

		writeLocked = true;
		ret = 0;
	}

	return ret;
}

/*
 * invokes an unlock request
 */
int Lock::unlock() {

	unique_lock<mutex> syncLock(*m);

	// in case of lock is a write lock or the last read lock notify one thread and reset member variables
	if (writeLocked || --readLocks == 0) {
		writeLocked = false;
		readLocks = 0;
		condVar->notify_one();
	}

	return 0;
}

/*
 * returns amount of waiting threads on current instance
 */
int Lock::getWaitingThreads() {
	return waitingThreads;
}

Lock::~Lock() {

}
