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
	m = new mutex;
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

	BufferFrame* frame;
	// try to access page as if it were handled by buffer manager

	// restrict access to hashmap to remove from queue before other threads can delete in reclaim attempt
	unique_lock<mutex> syncLock(*m);

	if (bufferFramesMap.count(pageId) > 0) {

		// pageId in memory - else go to catch (out of range exception)
		frame = bufferFramesMap.at(pageId);

		//cout << "In-Memory Page " << pageId << ", exclusive(" << exclusive << ")" << endl;

		// remove from queues
		fifoQ.remove(pageId);
		lruQ.remove(pageId);

		// successful access - block frame
		frame->lockFrame(exclusive);

		// unlock file access
		syncLock.unlock();

		//cout << "Page " << pageId << " fixed successfully" << endl;

	} else {
		// pageId not in memory
		//cout << "pageId: " << pageId << " produced page miss. Try to reclaim. " << endl;

		// try to reclaim a frame
		BufferFrame& bframe = reclaimFrame();

		if (&bframe != NULL) {

			//char* newBufferedData = (char *) operator new(pageSize);

			//frame = new BufferFrame(pageId, newBufferedData);

			// set page-id
			bframe.setPageId(pageId);

			// set state to new
			bframe.setState(BufferFrame::STATE_NEW);

			// add to fifo queue, since it is a fresh page
			bframe.setWhichQ(BufferFrame::Q_FIFO);

			// load page data into frame
			fsSource.seekg(pageId * pageSize);
			if (fsSource.read((char *) bframe.getData(), pageSize).fail()) {
				cerr << "Could not load page: " << pageId << " from file." << endl;
			}
			// insert frame into hashmap with new pageId
			bufferFramesMap.insert(make_pair(pageId, &bframe));

			// make sure pageId is in no queue
			fifoQ.remove(pageId);
			lruQ.remove(pageId);

			// lock frame access
			bframe.lockFrame(exclusive);

			// release file and hashmap access
			syncLock.unlock();

			return bframe;

		} else {
			// reclaimFrame() unsuccessful
			cerr << "All pages in use. Cannot reclaim any frame." << endl;
			syncLock.unlock();
			exit(-1);
		}
	}
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

	// release the lock on the frame
	frame.unlockFrame();

	// synchronize access to bufferFramesMap
	unique_lock<mutex> syncLock(*m);

	// perform unfix page only if there is no thread waiting for the current frame (otherwise the next thread does the unfix progress)
	if (frame.getAmountOfWaitingThreads() == 0) {

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

			// put unfixed page back into queue depending on where it was before
			if (frame.getWhichQ() == BufferFrame::Q_FIFO) {
				/*
				 * first load -> fifo queue (as seen in constructor and fixPage)
				 * first usage -> fifo queue but set up for lru next time
				 * nth usage -> lru queue
				 */
				fifoQ.remove(frame.getPageId()); // be sure that FIFO queue does not contain page-id
				lruQ.remove(frame.getPageId()); // also remove from lruQ for concurrency
				fifoQ.push_back(frame.getPageId()); // goes to FIFO queue one more time

				frame.setWhichQ(BufferFrame::Q_LRU); // next unfix it will move to lru
			} else {

				lruQ.remove(frame.getPageId());
				fifoQ.remove(frame.getPageId());
				lruQ.push_back(frame.getPageId());

				frame.setWhichQ(BufferFrame::Q_LRU);
			}

			// unlock access to bufferFramesMap
			syncLock.unlock();

			//cout << "Page " << frame.getPageId() << " unfixed successfully" << endl;

		} else {

			cerr << "Buffer manager contains no frame with page-id " << frame.getPageId() << endl;

			// unlock access to bufferFramesMap
			syncLock.unlock();
		}
	} else {
		// unlock access to bufferFramesMap
		syncLock.unlock();
	}
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

	if (fifoQ.size() > 0) {

		// try to free a frame from the fifo queue
		oldPageId = fifoQ.front();
		fifoQ.pop_front();
		fifoQ.remove(oldPageId);

		frameToReplaceAvailable = true;

	} else if (lruQ.size() > 0) {

		// try to free a frame from the lru queue
		oldPageId = lruQ.front();
		lruQ.pop_front();
		lruQ.remove(oldPageId);

		frameToReplaceAvailable = true;

	}

	if (frameToReplaceAvailable) {

		try {

			// get framepointer and then remove oldPageId reference from hashmap
			bufferFrame = bufferFramesMap.at(oldPageId);

			// if dirty write back to disk before erasing
			//if (bufferFrame->getState() == BufferFrame::STATE_DIRTY) { CAUSES TONS OF DAMN ERRORS
			writeBackToDisk(*bufferFrame);
			fsSource.flush();
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

/**
 * Destructor of the Buffer Manager
 */
BufferManager::~BufferManager() {

	for (auto it = bufferFramesMap.begin(); it != bufferFramesMap.end(); ++it) {

		if (it->second->getState() == BufferFrame::STATE_DIRTY) {

			// write dirty pages back to disk
			writeBackToDisk(*(it->second));

			fsSource.flush();
		}

		// delete buffer frames
		delete (it->second);
	}

	// close file stream to source file
	fsSource.close();

}
