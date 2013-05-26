/*
 * bufman.cpp
 *
 *  Created on: May 3, 2013
 *      Author: DRomanAvid
 */

#include "bufman.h"
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <mutex>

using namespace std;

mutex bufman_mutex;

/*
 * Constructor for BufferManager
 *
 * @param filename: name of file to be buffered as string
 * @param size: number of frames the buffer manager manages
 *
 */
BufferManager::BufferManager(const string& filename, uint64_t size) :
		fsSource(filename, fstream::in | fstream::out) {

	// check if file opening was successful
	if (fsSource.fail()) {
		cerr << "cannot open file " << filename << endl;
		exit(-1);
	}

	// calculate page size (in bytes) - usually 4096
	pageSize = sysconf(_SC_PAGESIZE);

	// write member attributes
	buffSize = size;
	fsSource.seekg(0, std::fstream::end);
	pagesOnDisk = fsSource.tellg() / pageSize;
	fsSource.seekg(0);

	// allocate size pages of memory for actual data
	bufferedData = (char *) operator new(buffSize * pageSize);

	// read file content into buffer (as much data as possible corresponding to buffer size)
	if (fsSource.read(bufferedData, buffSize * pageSize).fail()) {
		cerr << "cannot read file" << endl;
		exit(-1);
	}

	// instantiate buffSize buffer frames and put them into buffer frames map
	for (uint64_t i = 0; i < buffSize && i < pagesOnDisk; ++i) {

		// instantiate frame
		BufferFrame* frame = new BufferFrame(i, bufferedData + i * pageSize);

		// fill buffer frames map
		bufferFramesMap.insert(make_pair(i, frame));

		// fill fifo queue by page-id
		fifoQ.push_back(i);
	}

	cout << "Buffer-Manager ready" << endl;
}

/**
 * Tries to fix a page for use
 *
 * @param pageId: the id of the page to be fixed
 * @param exlusive: if true, exlusive lock the page, if false shared lock
 *
 * @return buffer frame containing the reference to the requested page
 */
BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive) {

	// restrict access to hashmap to remove from queue before other threads can delete in reclaim attempt
	//unique_lock<mutex> syncLock(*m);
	bufman_mutex.lock();

	BufferFrame *frame;

	if (bufferFramesMap.count(pageId) > 0) {

		// pageId in memory
		frame = bufferFramesMap.at(pageId);

		// try to lock the frame
		int success = frame->tryLockFrame(exclusive);

		if (success == -1) {
			frame->intent++;
			//unlock mutex to get around deadlock situation
			bufman_mutex.unlock();
			frame->lockFrame(exclusive);
			bufman_mutex.lock();
			frame->intent--;
		}

		if (pageId != frame->getPageId()) {
			cerr << "mega bullshit level, geht gornich, ne steife brise watt?" << endl;
		}

		// remove pageID out of queues
		fifoQ.remove(pageId);
		lruQ.remove(pageId);

		//cout << "Page " << pageId << " fixed successfully" << endl;

	} else {
		// pageId not in memory
		//cout << "pageId: " << pageId << " produced page miss. Try to reclaim. " << endl;

		// try to reclaim a frame
		BufferFrame& frame = reclaimFrame();

		if (&frame != NULL) {

			//char* newBufferedData = (char *) operator new(pageSize);

			//frame = new BufferFrame(pageId, newBufferedData);

			// try to lock the frame
			int success = frame.tryLockFrame(exclusive);

			if (success == -1) {
				frame.intent++;
				//unlock mutex to get around deadlock situation
				bufman_mutex.unlock();
				frame.lockFrame(exclusive);
				bufman_mutex.lock();
				frame.intent--;
			}


			// set page-id
			frame.setPageId(pageId);

			// set state to new
			frame.setState(BufferFrame::STATE_NEW);

			// add to fifo queue, since it is a fresh page
			frame.setWhichQ(BufferFrame::Q_FIFO);

			// load page data into frame
			fsSource.seekg(pageId * pageSize);
			if (fsSource.read((char *) frame.getData(), pageSize).fail()) {
				cerr << "Could not load page: " << pageId << " from file." << endl;
			}
			// insert frame into hashmap with new pageId
			bufferFramesMap.insert(make_pair(pageId, &frame));

			// release file and hashmap access
			//syncLock.unlock();
			bufman_mutex.unlock();

			return frame;

		} else {
			// reclaimFrame() unsuccessful
			cerr << "All pages in use. Cannot reclaim any frame." << endl;
			//syncLock.unlock();
			bufman_mutex.unlock();
			exit(-1);
		}
	}
	bufman_mutex.unlock();
	return *frame;

}

/**
 * tries to unfix a page
 *
 * @param frame: the frame pointing to the page
 * @param isDirty: if true, the page must be written to disk
 *
 */
void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {

	//cout << "Try to unfix page " << frame.getPageId() << " dirty = " << isDirty << endl;

	// synchronize access to bufferFramesMap
	//unique_lock<mutex> syncLock(*m);
	bufman_mutex.lock();

	// check if buffer frames map contains frame
	if (bufferFramesMap.count(frame.getPageId()) > 0) {

		// set state
		if (isDirty) {
			frame.setState(BufferFrame::STATE_DIRTY);
			// write frame-page back to disk
			writeBackToDisk(frame);
		} else {
			frame.setState(BufferFrame::STATE_CLEAN);
		}

		// remove pageID out of queues
		fifoQ.remove(frame.getPageId());
		lruQ.remove(frame.getPageId());

		// put unfixed page back into queue depending on where it was before
		if (frame.getWhichQ() == BufferFrame::Q_FIFO) {
			/*
			 * first load -> fifo queue (as seen in constructor and fixPage)
			 * first usage -> fifo queue but set up for lru next time
			 * nth usage -> lru queue
			 */
			fifoQ.push_back(frame.getPageId()); // goes to FIFO queue one more time
			frame.setWhichQ(BufferFrame::Q_LRU); // next unfix it will move to lru
		} else if (frame.getWhichQ() == BufferFrame::Q_LRU) {
			lruQ.push_back(frame.getPageId());
		} else {
			cerr << "*** error: queue of frame undetermined at unfix ***" << endl;
		}

		// release the lock on the frame
		frame.unlockFrame();

		//cout << "Page " << frame.getPageId() << " unfixed successfully" << endl;

	} else {

		cerr << "Buffer manager contains no frame with page-id " << frame.getPageId() << endl;
	}
	bufman_mutex.unlock();
}

/**
 * tries to write frame-data back to disk
 *
 * @param frame: the frame pointing to the page
 *
 */
void BufferManager::writeBackToDisk(BufferFrame& frame) {

	try {

		// set filestream to correct position
		fsSource.seekg(frame.getPageId() * pageSize);
		// write content
		if (fsSource.write((char*) frame.getData(), pageSize).fail()) {
			throw runtime_error("cannot write file");
		}

		fsSource.flush();

	} catch (exception& e) {
		cerr << "An exception occurred while writing data back to disk: " << e.what() << endl;
	}

}

/**
 * tries to swap a page in memory to disk
 *
 * @return: reclaimed buffer frame, null if no frame could be freed
 */
BufferFrame& BufferManager::reclaimFrame() {

	// oldPageId is the pageId of the page to be removed from memory
	uint64_t oldPageId;
	bool frameToReplaceAvailable = false;

	// bufferFrame is the reclaimed frame
	BufferFrame *bufferFrame;
	bufferFrame = NULL;

	while (!frameToReplaceAvailable) {
		if (fifoQ.size() > 0) {

			// try to free a frame from the fifo queue
			oldPageId = fifoQ.front();
			bufferFrame = bufferFramesMap.at(oldPageId);
			if (bufferFrame->intent == 0) {
				frameToReplaceAvailable = true;
			}

		} else if (lruQ.size() > 0) {

			// try to free a frame from the lru queue
			oldPageId = lruQ.front();
			bufferFrame = bufferFramesMap.at(oldPageId);
			if (bufferFrame->intent == 0) {
				frameToReplaceAvailable = true;
			}
		}
	}

	if (frameToReplaceAvailable) {

		try {

			// get framepointer and then remove oldPageId reference from hashmap
			bufferFrame = bufferFramesMap.at(oldPageId);

			// remove old page id from queue
			fifoQ.remove(oldPageId);
			lruQ.remove(oldPageId);

			// if dirty write back to disk before erasing
			//if (bufferFrame->getState() == BufferFrame::STATE_DIRTY) { CAUSES TONS OF DAMN ERRORS
			writeBackToDisk(*bufferFrame);
			//}

			// remove frame from frames-map
			bufferFramesMap.erase(oldPageId);

		} catch (const std::out_of_range& oor) {
			cerr << "***fatal error for pageId " << oldPageId << "*** (got page ID by queue which isn't in frames map)" << endl;
			exit(-1);
		}
	}

	return *bufferFrame;

}

uint64_t BufferManager::getPagesOnDisk(){
	return pagesOnDisk;
}

int BufferManager::getPageSize() {
	return pageSize;
}

/**
 * Destructor of the Buffer Manager
 */
BufferManager::~BufferManager() {

	for (auto it = bufferFramesMap.begin(); it != bufferFramesMap.end(); ++it) {

		if (it->second->getState() == BufferFrame::STATE_DIRTY) {

			// write dirty pages back to disk
			writeBackToDisk(*(it->second));
		}

		// delete buffer frames
		delete (it->second);
	}

	// close file stream to source file
	fsSource.close();

}
