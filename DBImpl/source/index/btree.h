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
	 * @param tidNode: the tidNode where tid was found (result-node)
	 *
	 * @return boolean which indicates whether the key was found or not
	 */
	bool lookupInternal(T key, Node<T, CMP>* node, TID& tid, LeafNode<T, CMP>& tidNode) {

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

			tidNode = *leaf;

		} else {

			InnerNode<T, CMP>* inner = reinterpret_cast<InnerNode<T, CMP>*>(node);

			unsigned i = 0;
			for (T iSeparator : inner->separators) {
				if (!(CMP()(iSeparator, key))) {

					// fetch childNode by child-pageId
					Node<T, CMP>* childNode = seg->readFromFrame<T, CMP>(inner->children.at(i));
					childNode->parentNode = node;
					childNode->pageId = inner->children.at(i);

					// recursive call
					rtrn = lookupInternal(key, childNode, tid, tidNode);
					break;
				}
				++i;
			}
		}

		return rtrn;
	}

	/**
	 * Searches the first leaf node on lowest level starting at given node
	 *
	 * @param node: the current node
	 *
	 * @return rtrn: first leaf node
	 */
	LeafNode<T, CMP>* fetchFirstLeaf(Node<T, CMP>* node) {

		LeafNode<T, CMP>* rtrn = NULL;

		if (node->isLeaf) {

			rtrn = reinterpret_cast<LeafNode<T, CMP>*>(node);

		} else {

			InnerNode<T, CMP>* inner = reinterpret_cast<InnerNode<T, CMP>*>(node);

			Node<T, CMP>* firstChildNode = seg->readFromFrame<T, CMP>(inner->children.at(0));

			rtrn = fetchFirstLeaf(firstChildNode);
		}

		return rtrn;
	}

	/**
	 * Calculates the free space within a node
	 *
	 * @param *node: the node whose space is calculated
	 *
	 * @return long int with the number of bytes free in the node
	 */
	long calculateFreeNodeSpace(Node<T, CMP> *node) {

		// fetch page size
		long rtrn = seg->getBm()->getPageSize();

		// add the node header
		rtrn -= sizeof(Node<T, CMP> );

		if (node->isLeaf) {

			// alternative: calculate keys.capacity() * sizeof(T)
			// and values.capacity() * sizeof(TID)
			rtrn -= sizeof(LeafNode<T, CMP> );
			rtrn -= node->count * (sizeof(T) + sizeof(TID));

		} else {

			// alternative: calculate separators.capacity() * sizeof(T)
			// and children.capacity() * sizeof(uint64_t)
			rtrn -= node->count * sizeof(T);
			rtrn -= (node->count + 1) * sizeof(uint64_t);

		}

		return rtrn;
	}

	/**
	 * Splits a node and cascades to the top of the b tree
	 *
	 * @param *node: the node to be split
	 */
	void splitNode(Node<T, CMP> *node) {

		if (node->isLeaf) {

			// create neighboring leaf and set next to it
			LeafNode<T, CMP>* thisLeaf = reinterpret_cast<LeafNode<T, CMP>*>(node);
			LeafNode<T, CMP>* neighborLeaf = new LeafNode<T, CMP>;
			neighborLeaf->isLeaf = true;
			neighborLeaf->parentNode = thisLeaf->parentNode;
			neighborLeaf->pageId = seg->getNewPageId();
			thisLeaf->nextPageId = neighborLeaf->pageId;
			thisLeaf->next = neighborLeaf;

			// take first half of keys and values and place them left
			// place the others on the neighborLeaf
			unsigned half = (thisLeaf->keys).size() / 2;
			for (unsigned i = half; i < (thisLeaf->keys).size(); ++i) {
				(neighborLeaf->keys).push_back(thisLeaf->keys.at(i));
				(neighborLeaf->values).push_back(thisLeaf->values.at(i));
				(neighborLeaf->count)++;
			}

			// delete half to end from key and value vectors of leaf
			thisLeaf->keys.erase((thisLeaf->keys).begin() + half, thisLeaf->keys.end());
			thisLeaf->values.erase((thisLeaf->values).begin() + half, thisLeaf->values.end());
			(thisLeaf->count) = thisLeaf->keys.size();

			// write changes back to disk
			if (!seg->writeToFrame<T, CMP>(thisLeaf, thisLeaf->pageId)) {
				cerr << "Cannot write thisLeaf into frame" << endl;
			}
			if (!seg->writeToFrame<T, CMP>(neighborLeaf, neighborLeaf->pageId)) {
				cerr << "Cannot write neighborLeaf into frame" << endl;
			}

			// check if parent needs to be split
			long neededSpace = sizeof(T) + sizeof(uint64_t);
			if (neededSpace > calculateFreeNodeSpace(thisLeaf->parentNode)) {
				splitNode(thisLeaf->parentNode); // implement child re-hanging in else case
			}

		} else {

			// split children -> update their parents
			// split vectors
		}
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
	void insert(T key, TID& tid) {

		LeafNode<T, CMP> insertNode;

		// calculate required space
		long requiredSpace = sizeof(TID) + sizeof(T);

		// because lookupInternal changes tid - this invokes copy constructor
		TID toInsertTID = tid;

		// lookup and change to the node which should be inserted to
		lookupInternal(key, rootNode, tid, insertNode);

		long freeNodeSpace = calculateFreeNodeSpace(&insertNode);

		// node should be a leaf node
		LeafNode<T, CMP> *leaf;

		if (insertNode.isLeaf) {
			leaf = reinterpret_cast<LeafNode<T, CMP>*>(&insertNode);
		} else {
			std::cerr << "** BTree::lookupInternal() returned an inner node **" << std::endl;
		}

		if (freeNodeSpace > requiredSpace) {

			uint16_t pos = 0;
			for (T lKey : leaf->keys) {
				// check if new key is larger
				if ((CMP()(lKey, key))) {
					++pos;
				} else {
					break;
				}
			}

			// add key and value at correct position
			(leaf->keys).insert((leaf->keys).begin() + pos, key);
			(leaf->values).insert((leaf->values).begin() + pos, tid);

			// increment counter
			(leaf->count)++;

			// write changes back to disk
			if (!seg->writeToFrame<T, CMP>(leaf, leaf->pageId)) {
				cerr << "Cannot write tree node into frame" << endl;
			}

		} else {
			// split the node and insert
			splitNode(leaf);
		}
	}

	/**
	 * Deletes a specified key from the tree
	 * (simplified mode)
	 *
	 * @param key: the key
	 */
	void erase(T key) {

		// search tid beginning at the root node
		LeafNode<T, CMP> foundInNode;
		TID tid;

		// removal has a successful lookup in advance
		if (lookupInternal(key, rootNode, tid, foundInNode)) {

			unsigned pos = 0;
			for (T lKey : foundInNode.keys) {
				// check for equality
				if (!(CMP()(lKey, key)) && !(CMP()(key, lKey))) {
					break;
				}
				++pos;
			}

			// remove key and value at correct position
			(foundInNode.keys).erase((foundInNode.keys).begin() + pos);
			(foundInNode.values).erase((foundInNode.values).begin() + pos);

			// decrement counter
			(foundInNode.count)--;

			// write changes back to disk
			if (!seg->writeToFrame<T, CMP>(&foundInNode, foundInNode.pageId)) {
				cerr << "Cannot write tree node into frame" << endl;
			}

		} else {
			cerr << "Key to delete was not found in tree" << endl;
		}
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
		LeafNode<T, CMP> foundInNode;
		return lookupInternal(key, rootNode, tid, foundInNode);
	}

	/**
	 * Returns an iterator that allows to iterate over the result set within (start,stop( range
	 *
	 * @return rtrn: the iterator
	 */
	vector<TID>::iterator lookupRange(T start, T stop) {

		vector<TID>* result = new vector<TID>();

		// search tid beginning at the root node
		LeafNode<T, CMP> foundInNode;
		TID tid;

		if (lookupInternal(start, rootNode, tid, foundInNode)) {

			bool stopReached = false;

			// iterate on leaf level through leaf nodes until end or stop is reached
			do {

				for (int i = 0; i < foundInNode.count; i++) {

					if (!(CMP()(foundInNode.keys.at(i), stop)) && !(CMP()(stop, foundInNode.keys.at(i)))) {
						stopReached = true;
						break;
					} else {
						result->push_back(foundInNode.values.at(i));
					}
				}

			} while ((foundInNode = seg->fetchNextLeaf<T, CMP>(foundInNode->nextPageId)) != NULL && !stopReached);

		} else {
			cerr << "Start key was not found in tree" << endl;
		}

		return result->begin();
	}

	/**
	 * Returns the size of the tree (amount of indexed TID's)
	 *
	 * @return rtrn: the size
	 */
	uint64_t size() {

		uint64_t rtrn = 0;

		// fetch first leaf node starting at root node
		LeafNode<T, CMP>* leaf = fetchFirstLeaf(rootNode);
		rtrn += leaf->count;

		// iterate on leaf level through leaf nodes and count size
		while ((leaf = seg->fetchNextLeaf<T, CMP>(leaf->nextPageId)) != NULL) {
			rtrn += leaf->count;
		}

		return rtrn;
	}

	virtual ~BTree() {
		delete rootNode;
	}
};

#endif
