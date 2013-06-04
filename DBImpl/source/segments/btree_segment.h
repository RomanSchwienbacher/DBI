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
		// FIXME go on here
		LeafNode<T, CMP>* leaf = new LeafNode<T, CMP>();
		return leaf;
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

	virtual ~BTreeSegment();
};

#endif
