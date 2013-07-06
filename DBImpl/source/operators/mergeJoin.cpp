/*
 * mergeJoin.cpp
 *
 *	Equi-joins two sorted inputs using the merge join algorithm (assuming corresponding attributes are already sorted)
 *
 *  Created on: Jul 6, 2013
 *      Author: DRomanAvid
 */

#include "mergeJoin.h"
#include <iostream>
#include <string>

MergeJoin::MergeJoin(SchemaSegment& schemaSeg) :
		Operator(schemaSeg, OperatorType::MERGE_JOIN) {
	inputOperatorLeft = NULL;
	inputOperatorRight = NULL;
	blockSize = 0;
	functionality = Functionality::ONE_ONE;
	oldLeft = NULL;
	oldRight = NULL;
	curLeft = NULL;
	curRight = NULL;
	fetchNextLeft = false;
	preventFetchingNext = false;
	n_mMatchState = N_M_MatchState::OFF;
}

void MergeJoin::open(Operator* inputOperatorLeft, Operator* inputOperatorRight, string attributeLeft, string attributeRight) {

	if (inputOperatorLeft != NULL && inputOperatorRight != NULL && !attributeLeft.empty() && !attributeRight.empty()) {

		MergeJoin::inputOperatorLeft = inputOperatorLeft;
		MergeJoin::inputOperatorRight = inputOperatorRight;
		MergeJoin::attributeLeft = attributeLeft;
		MergeJoin::attributeRight = attributeRight;

		MergeJoin::attributes = *(new vector<string>());
		// in case of attributes have the same name, keep only one attribute
		if (attributeLeft == attributeRight) {
			attributes.push_back(attributeLeft);
		} else {
			attributes.push_back(attributeLeft);
			attributes.push_back(attributeRight);
		}

		blockSize = attributes.size();

	} else {
		cerr << "Input operators and attributes must be given" << endl;
		throw;
	}
}

/**
 * calculates functionality of relation between current two input operators
 */
void MergeJoin::reCalculateFunctionality() {

	// re calculate functionality if current functionality is not n_m only (n-m is the maximum) and if old registers are available (not first run)
	if (functionality != Functionality::N_M && oldLeft != NULL && oldRight != NULL) {

		if (functionality == Functionality::ONE_ONE) {
			// fetchNextLeft says here if left side has been updated, only in this case it is necessary to check oldLeft and curLeft
			if (fetchNextLeft && oldLeft == curLeft) {
				functionality = Functionality::N_ONE;
			} else if (!fetchNextLeft && oldRight == curRight) {
				functionality = Functionality::ONE_N;
			}
		}

		if ((functionality == Functionality::N_ONE && !fetchNextLeft && oldRight == curRight) || (functionality == Functionality::ONE_N && fetchNextLeft && oldLeft == curLeft)) {
			functionality = Functionality::N_M;
		}
	}
}

/**
 * next operator for merge join: tries to find a join pair, if found return true, if not found search ahead (recursive) until both sides were processed completely
 * contains special behavior for n:m relationships
 */
bool MergeJoin::next() {

	// first clear all current register entries
	registerEntries.clear();

	// if current left register is null or advice to fetch next left, try to fetch
	if (!preventFetchingNext && (curLeft == NULL || fetchNextLeft)) {
		if (inputOperatorLeft->next()) {
			// then push registers which correspond to the attributes to register entries
			for (Register* reg : inputOperatorLeft->getOutput()) {
				if (find(attributes.begin(), attributes.end(), reg->getAttrName()) != attributes.end()) {
					oldLeft = curLeft;
					curLeft = reg;
					break;
				}
			}
		} else {
			// return false if left side must be go ahead but not possible
			return false;
		}
	}

	// if current right register is null or advice to fetch next right, try to fetch
	if (!preventFetchingNext && (curRight == NULL || !fetchNextLeft)) {
		if (inputOperatorRight->next()) {
			// then push registers which correspond to the attributes to register entries
			for (Register* reg : inputOperatorRight->getOutput()) {
				if (find(attributes.begin(), attributes.end(), reg->getAttrName()) != attributes.end()) {
					oldRight = curRight;
					curRight = reg;
					break;
				}
			}
		} else {
			// return false if right side must be go ahead but not possible
			return false;
		}
	}

	// prevent fetching can only be active for one iteration
	preventFetchingNext = false;

	// check type equality and attribute-availability
	if (curLeft == NULL || curRight == NULL || curLeft->getType() != curRight->getType()) {
		cerr << "Join attributes must have the same type or attribute names are not available" << endl;
		throw;
	}

	// recalculate functionality at runtime
	reCalculateFunctionality();

	// normal matching behavior if n:m match state is not 'BROKEN'
	if (n_mMatchState != N_M_MatchState::BROKEN) {

		// equi-join: check for equality
		if (curLeft == curRight) {
			for (unsigned i = 0; i < blockSize; i++) {
				registerEntries.push_back(curLeft);
			}
			fetchNextLeft = true;

			// enter the n-m match state 'ACTIVE' if whe have a join pair and a functionality of n:m
			if (functionality == Functionality::N_M) {
				n_mMatchState = N_M_MatchState::ACTIVE;
			}

			// join pair found, so return true
			return true;
		}
		// if not equal perform recursive next call
		else {

			// enter the n-m match state 'BROKEN' if it was active but now the join partner does not match any more
			if (n_mMatchState == N_M_MatchState::ACTIVE) {
				n_mMatchState = N_M_MatchState::BROKEN;
			}

			if (curLeft < curRight) {
				fetchNextLeft = true;
			} else {
				fetchNextLeft = false;
			}
			// recursive call
			return next();
		}
	}
	// n:m match type is 'BROKEN' so compare the still-matching-side with the old register of the other side until the other side does not match this value any more
	else {
		// equi-join: check for equality
		if ((fetchNextLeft && curLeft == oldRight) || (!fetchNextLeft && oldLeft == curRight)) {
			for (unsigned i = 0; i < blockSize; i++) {
				registerEntries.push_back(fetchNextLeft ? curLeft : curRight);
			}

			// stay in the n-m match state 'BROKEN' and keep fetchNextLeft value

			// join pair found, so return true
			return true;
		}
		// if not equal perform recursive next call
		else {

			// enter the n-m match state 'OFF'
			n_mMatchState = N_M_MatchState::OFF;

			// next iteration prevent fetching because we already have our current register values
			preventFetchingNext = true;

			// recursive call
			return next();
		}
	}
}

vector<Register*> MergeJoin::getOutput() {
	return registerEntries;
}

void MergeJoin::close() {

	inputOperatorLeft->close();
	inputOperatorRight->close();

	inputOperatorLeft = NULL;
	inputOperatorRight = NULL;
	attributes.clear();
	registerEntries.clear();
	blockSize = 0;
	functionality = Functionality::ONE_ONE;
	oldLeft = NULL;
	oldRight = NULL;
	curLeft = NULL;
	curRight = NULL;
	fetchNextLeft = false;
	preventFetchingNext = false;
	n_mMatchState = N_M_MatchState::OFF;
}

vector<string> MergeJoin::getAttributes() {
	return attributes;
}

unsigned MergeJoin::getBlocksize() {
	return blockSize;
}

MergeJoin::~MergeJoin() {

}

