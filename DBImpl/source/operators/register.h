//
//  Register.h
//  Operator
//
//  Created by David Grotzky on 6/17/13.
//  Copyright (c) 2013 David Grotzky. All rights reserved.
//

#ifndef __Operator__Register__
#define __Operator__Register__

#include <iostream>
#include <algorithm>
#include "../../parsinglib/Types.hpp"

template<class T>

class Register {

	// data pointer
	char* data;

	// data type vector
	Types::Tag dataType;

	// char length - if char stored
	unsigned charLen;

	// boolean for initialization
	bool isNew = true;

public:

	// Constructors
	Register(uint64_t size);

	Register(int val);

	template<unsigned len>
	Register(const Char<len> val) {
		// set dataType of register
		dataType = Types::Tag::Char;

		// set to value
		setChar(val);

		isNew = false;

	}

	// Comparators
	friend bool operator<(const Register &reg1, const Register &reg2) {

		bool ret = false;

		// compare integers
		if (reg1.dataType == Types::Tag::Integer && reg2.dataType == Types::Tag::Integer) {
			ret = reg1.getInteger() < reg2.getInteger();
		}
		// compare chars
		else if (reg1.dataType == Types::Tag::Char && reg2.dataType == Types::Tag::Char) {
			if (reg1.charLen != reg2.charLen) {
				int len = std::min(reg1.charLen, reg2.charLen);
				int result = memcmp(reg1.data, reg2.data, len);
				if(result != 0){
					ret = result < 0;
				}
			} else{
				ret = memcmp(reg1.data, reg2.data, reg1.charLen) < 0;
			}

		}
		// if one is integer and the other Char
		else if (reg1.dataType != reg2.dataType) {
			ret = (reg1.dataType == Types::Tag::Integer);
		}

		return ret;
	}

	friend bool operator==(const Register &reg1, const Register &reg2) {

		bool ret = false;

		// compare integers
		if (reg1.dataType == Types::Tag::Integer && reg2.dataType == Types::Tag::Integer) {
			ret = reg1.getInteger() == reg2.getInteger();
		}
		// compare chars
		else if (reg1.dataType == Types::Tag::Char && reg2.dataType == Types::Tag::Char) {
			if (reg1.charLen == reg2.charLen) {
				ret = memcmp(reg1.data, reg2.data, reg1.charLen) == 0;
			}

		}
		// if one is integer and the other Char -> stay false
	}

// Getters
	int getInteger() const;

	template<unsigned len>
	Char<len> getChar() {
		Char<len> ret;

		// data pointer cast to Char<len>
		if (dataType == Types::Tag::Char) {
			memcpy(ret.data, data, len);
		}

		return ret;
	}

	Types::Tag getType() const;

	unsigned getCharLength() const;

// Setters
	void setInteger(int val);

	template<unsigned len>
	void setChar(const Char<len> val) {
		if (dataType == Types::Tag::Char) {

			// remove old data - if applicable
			if (!isNew)
				delete[] data;

			// set char length
			charLen = len;

			// allocate space
			data = new char[charLen];

			// copy char into memory
			memcpy(data, val.data, charLen);
		}
	}

// Destructor
	~Register();

};

#endif /* defined(__Operator__Register__) */
