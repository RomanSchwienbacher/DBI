/*
 * btree.h
 *
 * Index structure: B+Tree
 * It's a template class, therefore provide interface and implementation in one single header file
 *
 *  Created on: Jun 2, 2013
 *      Author: DRomanAvid
 */

#ifndef BTREE_H_
#define BTREE_H_

#include <iterator>
#include <vector>
#include <inttypes.h>
#include "../segments/btree_segment.h"
#include "../segments/tid.h"
#include "node.h"

template<class T, class CMP>
class BTree {

	BTreeSegment* seg;
	Node<T, CMP>* rootNode;

public:

	// jede page wird exklusiv geladen

	/**
	 * Constructor: initialize seg and rootNode
	 *
	 * @param seg: B+Tree Segment
	 */
	BTree(BTreeSegment& seg) {
		BTree::seg = &seg;
		BTree::rootNode = seg.initializeRootNode<T, CMP>();
	}

	/**
	 * Inserts a new key/TID pair into the tree
	 *
	 * @param key: the key
	 * @param tid: the TID
	 */
	void insert(T key, TID tid) {

	}

	/**
	 * Deletes a specified key from the tree
	 * mach die einfache variante!!!!
	 *
	 * @param key: the key
	 */
	void erase(T key) {

	}

	/**
	 * Searches a TID by key and writes found-tid into referenced param tid
	 *
	 * @param key: the key
	 * @param tid: the tid (result)
	 *
	 * @return rtrn: indicates that the key was found or not
	 */
	bool lookup(T key, TID& tid) {
		return false;
	}

	/**
	 * Returns an iterator that allows to iterate over the result set
	 *
	 * @return rtrn: the iterator
	 */
	vector<TID>::iterator lookupRange() {
		return NULL;
	}

	/**
	 * Returns the size of the tree (amount of nodes)
	 *
	 * @return rtrn: the size
	 */
	uint64_t size() {
		return 0;
	}

	virtual ~BTree() {
		delete rootNode;
	}
};

#endif
