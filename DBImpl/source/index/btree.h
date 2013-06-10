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
				if (!(CMP()(iSeparator, key)) || (i+1) == inner->separators.size()) {

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
			rtrn -= sizeof(uint64_t); // nextPageId
			rtrn -= sizeof(LeafNode<T, CMP>*); // next pointer
			rtrn -= node->count * (sizeof(T) + sizeof(TID)); // current entries

		} else {

			// alternative: calculate separators.capacity() * sizeof(T)
			// and children.capacity() * sizeof(uint64_t)
			rtrn -= sizeof(uint64_t); // next page Id
			rtrn -= sizeof(InnerNode<T, CMP>*); // next pointer
			rtrn -= node->count * sizeof(T);
			rtrn -= (node->count + 1) * sizeof(uint64_t);

		}

		return rtrn;
	}

	/**
	 * Inserts a key and value into a certain node
	 *
	 * @param key: the key to be inserted at the correct position
	 * @param tid: the tid value to be placed at the respective key position in a leaf
	 * @param childPage: the pageId of the child page to be placed in an inner node
	 * @param *node: the node in which to place the key/value pair
	 * @return boolean to indicate success or failure of insert
	 */
	bool insertKeyValueIntoNode(T key, TID tid, uint64_t childPage, Node<T, CMP>* node) {

		bool rtrn = true;

		if (node->isLeaf) {

			LeafNode<T, CMP>* leaf = reinterpret_cast<LeafNode<T, CMP>*>(node);
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
				cerr << "Cannot write tree leaf into frame" << endl;
				rtrn = false;
			}

		} else {
			// node is an inner node
			InnerNode<T, CMP>* innerNode = reinterpret_cast<InnerNode<T, CMP>*>(node);

			uint16_t pos = 0;
			for (T lSep : innerNode->separators) {
				// check if new separator is larger
				if ((CMP()(lSep, key))) {
					++pos;
				} else {
					break;
				}
			}

			// add key and value at correct position
			(innerNode->separators).insert((innerNode->separators).begin() + pos, key);
			(innerNode->children).insert((innerNode->children).begin() + pos, childPage);

			// increment counter
			(innerNode->count)++;

			// write changes back to disk
			if (!seg->writeToFrame<T, CMP>(innerNode, innerNode->pageId)) {
				cerr << "Cannot write tree node into frame" << endl;
				rtrn = false;
			}

		}

		return rtrn;
	}

	/**
	 * Splits a node and cascades to the top of the b tree
	 *
	 * @param *node: the node to be split
	 */
	void splitNode(Node<T, CMP> *node) {

		// Check if we are at root - create new root
		if (node->parentNode == NULL) {
			rootNode = new InnerNode<T, CMP>();
			rootNode->count = 0;
			rootNode->isLeaf = false;
			rootNode->pageId = seg->getNewPageId();
			rootNode->parentNode = NULL;
			node->parentNode = rootNode;

			// write root node to disk
			if (!seg->writeToFrame<T, CMP>(rootNode, rootNode->pageId)) {
				cerr << "Cannot write rootNode into frame" << endl;
			}
		}

		if (node->isLeaf) {

			// create neighboring leaf and set next to it
			LeafNode<T, CMP>* thisLeaf = reinterpret_cast<LeafNode<T, CMP>*>(node);
			LeafNode<T, CMP>* neighborLeaf = new LeafNode<T, CMP>();
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

		} else {
			// node is inner node

			// create neighboring inner node and set it up
			InnerNode<T, CMP>* innerNode = reinterpret_cast<InnerNode<T, CMP>*>(node);
			InnerNode<T, CMP>* newInnerNode = new InnerNode<T, CMP>();
			newInnerNode->isLeaf = false;
			newInnerNode->parentNode = innerNode->parentNode;
			newInnerNode->pageId = seg->getNewPageId();
			newInnerNode->next = NULL;
			innerNode->next = newInnerNode;

			// split children and update their parents
			// take first half of separators and children and place them left
			// place the others on the newInnerNode
			unsigned half = (innerNode->separators).size() / 2;
			for (unsigned i = half; i < (innerNode->separators).size(); ++i) {
				(newInnerNode->separators).push_back(innerNode->separators.at(i));
				(newInnerNode->children).push_back(innerNode->children.at(i));
				(newInnerNode->count)++;

				// update parent
				Node<T, CMP>* childNode = seg->readFromFrame<T, CMP>(newInnerNode->children.at(i));
				childNode->parentNode = newInnerNode;

				// write changes back to disk (each child)
				if (!seg->writeToFrame<T, CMP>(childNode, childNode->pageId)) {
					cerr << "Cannot write childNode into frame" << endl;
				}
			}

			// delete half to end from key and value vectors of inner node
			innerNode->separators.erase((innerNode->separators).begin() + half, innerNode->separators.end());
			innerNode->children.erase((innerNode->children).begin() + half, innerNode->children.end());
			(innerNode->count) = innerNode->separators.size();

			// write changes back to disk
			if (!seg->writeToFrame<T, CMP>(innerNode, innerNode->pageId)) {
				cerr << "Cannot write innerNode into frame" << endl;
			}
			if (!seg->writeToFrame<T, CMP>(newInnerNode, newInnerNode->pageId)) {
				cerr << "Cannot write newInnerNode into frame" << endl;
			}

			if (calculateFreeNodeSpace(innerNode->parentNode) < 0) {
				splitNode(innerNode->parentNode);
			}

		}
	}

	/**
	 * Iterates through B-Tree (used for visualize)
	 */
	Node<T, CMP>* iterateBTree(Node<T, CMP>* node) {
		Node<T, CMP>* rtrn;

		if (node->isLeaf) {
			LeafNode<T, CMP>* leaf = reinterpret_cast<LeafNode<T, CMP>*>(node);
			rtrn = leaf->next;
		} else {
			InnerNode<T, CMP>* innerNode = reinterpret_cast<InnerNode<T, CMP>*>(node);
		}

		return rtrn;
	}

public:

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

		// calculate required space: value and key
		long requiredSpace = sizeof(TID) + sizeof(T);

		// because lookupInternal changes tid - this invokes copy constructor
		TID toInsertTID = tid;

		// refetch rootNode
		rootNode = seg->readFromFrame<T, CMP>(rootNode->pageId);

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

			insertKeyValueIntoNode(key, tid, 0ul, leaf);

		} else {
			// split the node and insert
			splitNode(leaf);

			// insert key and value on proper side
			if ((CMP()(key, ((leaf->next)->keys).front()))) {
				// insert into leaf node
				insertKeyValueIntoNode(key, tid, 0ul, leaf);
			} else {
				// insert into next node
				insertKeyValueIntoNode(key, tid, 0ul, leaf->next);
			}

			// insert maximum of left page as separator into parent
			TID empty;
			insertKeyValueIntoNode(leaf->keys.back(), empty, leaf->pageId, leaf->parentNode);
			if (calculateFreeNodeSpace(leaf->parentNode) < 0) {
				// parent node overflow
				splitNode(leaf->parentNode);
			}

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

		// refetch rootNode
		rootNode = seg->readFromFrame<T, CMP>(rootNode->pageId);

		// fetch first leaf node starting at root node
		LeafNode<T, CMP>* leaf = fetchFirstLeaf(rootNode);
		rtrn += leaf->count;

		// iterate on leaf level through leaf nodes and count size
		while ((leaf = seg->fetchNextLeaf<T, CMP>(leaf->nextPageId)) != NULL) {
			rtrn += leaf->count;
		}

		return rtrn;
	}

	/**
	 * returns a dot syntax string representing the current btree
	 */
	std::string visualize() {
		std::stringstream stream1;
		std::stringstream stream2;

		stream1 << "digraph myBTree {" << endl;
		stream1 << "node [shape=record]" << endl;

		long nodeCount = 0;
		long childNodeCount = 1;
		long leafCount = 0;
		long childLeafCount = 0;

		Node<T, CMP>* currentNode;

		// start at root node
		currentNode = rootNode;

		bool endReached = false;

		while (!endReached) {

			if (currentNode->isLeaf) {
				LeafNode<T, CMP>* leaf = reinterpret_cast<LeafNode<T, CMP>*>(currentNode);

				// calculate maximum length of key/values
				LeafNode<T, CMP> emptyLeaf;
				long vectorSizeMax = calculateFreeNodeSpace(emptyLeaf) / (sizeof(T) + sizeof(TID));

				stream1 << "leaf" << leafCount << " [shape=record], label=" << "<count> " << leaf->count << " | <isLeaf> true | ";

				// add <key> tags
				for (long i = 0; i < vectorSizeMax; ++i) {

					stream1 << "<key" << i << "> ";
					if (i < leaf->keys.size()) {
						// existing key value
						stream1 << leaf->keys.at(i) << " | "; // TODO: check if this always outputs toString()
					} else {
						// emtpy key value
						stream1 << "| ";
					}
				}

				// add <tid> tags
				for (long i = 0; i < vectorSizeMax; ++i) {
					stream1 << "<tid" << i << "> ";
					if (i < leaf->values.size()) {
						// existing TID TODO: does this give the correct representation?
						stream1 << leaf->values.at(i) << " | ";
					} else {
						// empty value
						stream1 << "| ";
					}
				}

				// add <next> tag
				stream1 << "<next>";
				if (leaf->next != NULL) {
					stream1 << " *\"];" << endl;

					// add bottom part to stream2
					stream2 << "leaf" << leafCount << ":next -> leaf" << leafCount + 1 << ":count;" << endl;
				} else {
					stream1 << "\"];" << endl;
				}

				// increment leaf count
				leafCount++;

				if (leaf->next == NULL) {
					endReached = true;
				} else {
					// go to next node
					currentNode = leaf->next;
				}

			} else {
				// the current node is an inner node
				InnerNode<T, CMP>* innerNode = reinterpret_cast<InnerNode<T, CMP>*>(currentNode);

				// find out if this inner node has leaf or more inner node children - important for stream2 output
				Node<T, CMP>* childNode = seg->readFromFrame(innerNode->children.front());

				bool childIsLeaf = childNode->isLeaf;

				// calculate maximum length of key/values
				InnerNode<T, CMP> emptyInnerNode;
				long vectorSizeMax = calculateFreeNodeSpace(emptyInnerNode) / (sizeof(T) + sizeof(uint64_t));

				stream1 << "node" << nodeCount << " [shape=record], label=" << "<count> " << innerNode->count << " | <isLeaf> false | ";

				// add <key> tags
				for (long i = 0; i < vectorSizeMax - 1; ++i) {

					stream1 << "<key" << i << "> ";
					if (i < innerNode->separators.size()) {
						// existing key value
						stream1 << innerNode->separators.at(i) << " | "; // TODO: check if this always outputs toString()
					} else {
						// emtpy key value
						stream1 << "| ";
					}
				}

				// add <ptr> tags
				for (long i = 0; i < vectorSizeMax; ++i) {
					stream1 << "<ptr" << i << "> ";
					if (i < innerNode->children.size()) {
						// existing ptr TODO: does this give the correct representation?
						stream1 << "*";

						// add bottom part to stream2
						stream2 << "node" << nodeCount << ":ptr" << i << " -> ";
						if (childIsLeaf) {
							stream2 << "leaf" << childLeafCount++ << ":count;" << endl;
						} else {
							stream2 << "node" << childNodeCount++ << ":count;" << endl;
						}

					}
					if (i == vectorSizeMax - 1) {
						// end of TIDs
						stream1 << "\"];" << endl;
					} else {
						// more TIDs
						stream1 << " | ";
					}
				}

				// go to next inner node
				if (innerNode->next == NULL) {
					// go to first child
					currentNode = childNode;
				} else {
					currentNode = innerNode->next;
				}

				// increment nodeCount
				nodeCount++;

			}

		}

		stream1 << stream2.str() << "}" << endl;

		return stream1.str();
	}

	virtual ~BTree() {
		delete rootNode;
	}
};

#endif
