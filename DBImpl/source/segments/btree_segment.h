/*
 * btree_segment.h
 *
 *  Created on: June 2, 2013
 *      Author: DRomanAvid
 */

#ifndef BTREE_SEGMENT_H_
#define BTREE_SEGMENT_H_

#include "segment.h"
#include "tid.h"
#include "../index/node.h"

using namespace std;

class BTreeSegment: public Segment {

public:

	BTreeSegment(vector<uint64_t> freeExtents, uint64_t segId, FSISegment *fsi, BufferManager * bm);

	/**
	 * Initializes root node for B+Tree
	 */
	template<class T, class CMP>
	Node<T, CMP>* initializeRootNode() {
		// FIXME go on here
		LeafNode<T, CMP>* leaf = new LeafNode<T, CMP>();
		return leaf;
	}

	virtual ~BTreeSegment();
};

#endif
