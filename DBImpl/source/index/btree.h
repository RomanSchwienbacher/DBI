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
	bool lookupInternal(T key, Node<T, CMP>* node, TID& tid, Node<T, CMP>& tidNode) {

		bool rtrn = false;

		if (node->isLeaf) {

			LeafNode<T, CMP>* leaf = reinterpret_cast<LeafNode<T, CMP>*>(node);

			unsigned i = 0;
			for (T lKey : leaf->keys) {
				// check for equality
				if (!(CMP()(lKey, key)) && !(CMP()(key, lKey))) {
					tid = leaf->values.at(i);
					tidNode = *leaf;
					rtrn = true;
					break;
				}
				++i;
			}

		} else {

			InnerNode<T, CMP>* inner = reinterpret_cast<InnerNode<T, CMP>*>(node);

			unsigned i = 0;
			for (T iSeparator : inner->separators) {
				if (!(CMP()(iSeparator, key))) {
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
			thisLeaf->next = neighborLeaf;

			// take first half of keys and values and place them left
			// place the others on the neighborLeaf
			unsigned half = (thisLeaf->keys).size() / 2;
			for (unsigned i = half; i < (thisLeaf->keys).size(); ++i) {
				(neighborLeaf->keys).push_back(thisLeaf->keys.at(i));
				(neighborLeaf->values).push_back(thisLeaf->values.at(i));
			}

			// delete half to end from key and value vectors of leaf
			thisLeaf->keys.erase((thisLeaf->keys).begin() + half, thisLeaf->keys.end());
			thisLeaf->values.erase((thisLeaf->values).begin() + half, thisLeaf->values.end());

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

		bool test = false;

		/*
		Node<T, CMP> insertNode;

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

		} else {
			// split the node and insert
			splitNode(leaf);
		}
		*/
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
		Node<T, CMP> foundInNode;
		return lookupInternal(key, rootNode, tid, foundInNode);
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
