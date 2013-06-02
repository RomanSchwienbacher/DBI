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

template<class T, class CMP>
class BTree {

public:

	BTreeSegment* seg;

	BTree(BTreeSegment& seg);

	void insert(T key, TID tid);
	void erase(T key);
	bool lookup(T key, TID& tid);
	vector<TID>::iterator lookupRange();
	uint64_t size();

	virtual ~BTree();
};

/**
 * Constructor
 *
 * @param seg: B+Tree Segment
 */
template<class T, class CMP>
BTree<T, CMP>::BTree(BTreeSegment& seg) {
	BTree::seg = &seg;
}

/**
 * Inserts a new key/TID pair into the tree
 *
 * @param key: the key
 * @param tid: the TID
 */
template<class T, class CMP>
void BTree<T, CMP>::insert(T key, TID tid) {

}

/**
 * Deletes a specified key from the tree
 *
 * @param key: the key
 */
template<class T, class CMP>
void BTree<T, CMP>::erase(T key) {

}

/**
 * Searches a TID by key and writes found-tid into referenced param tid
 *
 * @param key: the key
 * @param tid: the tid (result)
 *
 * @return rtrn: indicates that the key was found or not
 */
template<class T, class CMP>
bool BTree<T, CMP>::lookup(T key, TID& tid) {

}

/**
 * Returns an iterator that allows to iterate over the result set
 *
 * @return rtrn: the iterator
 */
template<class T, class CMP>
vector<TID>::iterator BTree<T, CMP>::lookupRange() {

}

/**
 * Returns the size of the tree (amount of nodes)
 *
 * @return rtrn: the size
 */
template<class T, class CMP>
uint64_t BTree<T, CMP>::size() {

}

template<class T, class CMP>
BTree<T, CMP>::~BTree() {
}

#endif
