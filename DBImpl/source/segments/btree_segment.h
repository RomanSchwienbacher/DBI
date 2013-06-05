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
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <iostream>

using namespace std;

class BTreeSegment: public Segment {

	/**
	 * @return the serialized object (isLeaf;LSN;count;([Key;Value;]|[Separator;Child;])*)
	 */
	template<class T, class CMP>
	char* getSerialized(Node<T, CMP>* node) {

		char *rtrn = new char[sysconf(_SC_PAGESIZE)];
		int offset = 0;

		// serialize type
		memcpy(rtrn + offset, &(node->isLeaf), sizeof(bool));
		offset += sizeof(bool);

		// serialize LSN
		memcpy(rtrn + offset, &(node->LSN), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		// serialize count
		memcpy(rtrn + offset, &(node->count), sizeof(uint16_t));
		offset += sizeof(uint16_t);

		// node is leaf
		if (node->isLeaf) {

			LeafNode<T, CMP>* leaf = reinterpret_cast<LeafNode<T, CMP>*>(node);

			unsigned i = 0;
			for (T lKey : leaf->keys) {

				// serialize key
				memcpy(rtrn + offset, &lKey, sizeof(T));
				offset += sizeof(T);

				// serialize value
				memcpy(rtrn + offset, &(leaf->values.at(i)), sizeof(TID));
				offset += sizeof(TID);

				++i;
			}

		}
		// node is inner node
		else {

			InnerNode<T, CMP>* inner = reinterpret_cast<InnerNode<T, CMP>*>(node);

			unsigned i = 0;
			for (T sep : inner->separators) {

				// serialize separator
				memcpy(rtrn + offset, &sep, sizeof(T));
				offset += sizeof(T);

				// serialize child pointer
				memcpy(rtrn + offset, &(inner->children.at(i)), sizeof(uint64_t));
				offset += sizeof(uint64_t);

				++i;
			}
		}

		if (offset > sysconf(_SC_PAGESIZE)) {
			cerr << "SlottedPage::getSerialized(): Offset " << offset << " is bigger than the size of a single page" << endl;
		}

		return rtrn;
	}

	/**
	 * Factory: Parses a tree node instance by a given pointer
	 *
	 * @param tnPointer: the pointer
	 *
	 * @return rtrn: tree node instance pointer
	 */
	template<class T, class CMP>
	Node<T, CMP>* getDeserialized(char* tnPointer) {

		Node<T, CMP>* rtrn = NULL;

		int offset = 0;

		// first of all: deserialize type
		bool isLeaf = false;
		memcpy(&isLeaf, tnPointer + offset, sizeof(bool));
		offset += sizeof(bool);

		// node is leaf
		if (isLeaf) {

			LeafNode<T, CMP>* leaf = new LeafNode<T, CMP>();

			leaf->isLeaf = true;

			// deserialize LSN
			memcpy(&(leaf->LSN), tnPointer + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			// deserialize count
			memcpy(&(leaf->count), tnPointer + offset, sizeof(uint16_t));
			offset += sizeof(uint16_t);

			// deserialize keys and values
			for (int i = 0; i < leaf->count; i++) {

				T key;
				memcpy(&key, tnPointer + offset, sizeof(T));
				offset += sizeof(T);

				TID value;
				memcpy(&value, tnPointer + offset, sizeof(TID));
				offset += sizeof(TID);

				leaf->keys.insert((leaf->keys).begin() + i, key);
				leaf->values.insert((leaf->values).begin() + i, value);
			}

			rtrn = leaf;
		}
		// node is inner node
		else {

			InnerNode<T, CMP>* inner = new InnerNode<T, CMP>();

			inner->isLeaf = false;

			// deserialize LSN
			memcpy(&(inner->LSN), tnPointer + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			// deserialize count
			memcpy(&(inner->count), tnPointer + offset, sizeof(uint16_t));
			offset += sizeof(uint16_t);

			// deserialize keys and values
			for (int i = 0; i < inner->count; i++) {

				T separator;
				memcpy(&separator, tnPointer + offset, sizeof(T));
				offset += sizeof(T);

				uint64_t child;
				memcpy(&child, tnPointer + offset, sizeof(uint64_t));
				offset += sizeof(uint64_t);

				inner->separators.insert((inner->separators).begin() + i, separator);
				inner->children.insert((inner->children).begin() + i, child);
			}

			rtrn = inner;
		}

		return rtrn;
	}

public:

	BTreeSegment(vector<uint64_t> freeExtents, uint64_t segId, FSISegment *fsi, BufferManager * bm);

	/**
	 * Initializes root node for B+Tree
	 */
	template<class T, class CMP>
	Node<T, CMP>* initializeRootNode() {

		LeafNode<T, CMP>* root = new LeafNode<T, CMP>();
		root->isLeaf = true;
		root->parentNode = NULL;
		root->count = 0;
		root->pageId = getNextPageId(); // FIXME Dave, is this ok?

		// write root back to disk
		if (!writeToFrame(root, root->pageId)) {
			cerr << "Cannot write root node into frame" << endl;
		}

		return root;
	}

	/**
	 * Writes a tree node into a given buffer-frame
	 *
	 * @param node: the node
	 * @param pageId: the page id
	 *
	 * @return rtrn: whether successfully or not
	 */
	template<class T, class CMP>
	bool writeToFrame(Node<T, CMP>* node, uint64_t pageId) {

		bool rtrn = true;

		BufferFrame frame = bm->fixPage(pageId, true);

		try {

			// 1st step: serialize
			char* spSer = getSerialized<T, CMP>(node);

			// 2nd step: write into frame data pointer
			memcpy(frame.getData(), spSer, bm->getPageSize());

		} catch (exception& e) {
			cerr << "An exception occurred while writing tree node to frame: " << e.what() << endl;
			rtrn = false;
		}

		bm->unfixPage(frame, rtrn);

		return rtrn;
	}

	/**
	 * Reads a tree node from buffer-frame by pageId
	 *
	 * @param pageId: the page id
	 *
	 * @return rtrn: the tree node
	 */
	template<class T, class CMP>
	Node<T, CMP>* readFromFrame(uint64_t pageId) {

		Node<T, CMP>* rtrn = NULL;

		BufferFrame frame = bm->fixPage(pageId, false);

		try {
			//deserialize
			rtrn = getDeserialized<T, CMP>((char*) frame.getData());

		} catch (exception& e) {
			cerr << "An exception occurred while reading tree node from frame: " << e.what() << endl;
			rtrn = NULL;
		}

		bm->unfixPage(frame, false);

		return rtrn;
	}

	uint64_t getNextPageId();

	virtual ~BTreeSegment();
};

#endif
