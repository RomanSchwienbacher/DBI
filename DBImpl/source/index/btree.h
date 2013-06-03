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

	/**
	 * Searches tid by key recursively through b+tree
	 *
	 * @param key: the key
	 * @param node: the current node where key is searched
	 * @param tid: the tid (result)
	 *
	 * @return boolean which indicates whether the key was found or not
	 */
	bool lookupInternal(T key, Node<T, CMP>* node, TID& tid) {

		bool rtrn = false;

		if (node->isLeaf) {

			LeafNode<T, CMP>* leaf = reinterpret_cast<LeafNode<T, CMP>*>(node);

			unsigned i = 0;
			for (T lKey : leaf->keys) {
				// check for equality
				if (!(CMP()(lKey, key)) && !(CMP()(key, lKey))) {
					tid = leaf->values.at(i);
					rtrn = true;
					break;
				}
				++i;
			}

		} else {

			InnerNode<T, CMP>* inner = reinterpret_cast<InnerNode<T, CMP>*>(node);

			unsigned i = 0;
			for (T iSeparator : inner->separators) {
				if (!(CMP()(iSeparator, key)) ) {
					// recursive call
					// TODO fetch node by inner->children.at(i)
					//rtrn = lookupInternal(key, inner->children.at(i), tid);
					break;
				}
				++i;
			}
		}

		return rtrn;
	}

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
	 * @return boolean which indicates whether the key was found or not
	 */
	bool lookup(T key, TID& tid) {
		// search tid beginning at the root node
		return lookupInternal(key, rootNode, tid);
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
