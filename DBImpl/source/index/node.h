/*
 * node.h
 *
 * Node for B+Tree
 *
 *  Created on: Jun 2, 2013
 *      Author: DRomanAvid
 */

#ifndef NODE_H_
#define NODE_H_

# include <vector>
# include "../segments/tid.h"

template<class T, class CMP>
struct Node {
	uint32_t LSN;
	uint16_t count;
	bool isLeaf;
};

template<class T, class CMP>
struct InnerNode: public Node<T, CMP> {
	vector<T> separator;
	vector<uint64_t> children;
};

template<class T, class CMP>
struct LeafNode: public Node<T, CMP> {
	LeafNode* next;
	vector<T> keys;
	vector<TID> values;
};

#endif
