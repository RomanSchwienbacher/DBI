//
//  Register.cpp
//  Operator
//
//  Created by David Grotzky on 6/17/13.
//  Copyright (c) 2013 David Grotzky. All rights reserved.
//

#include "register.h"

/**
 * Constructs a Register with size bytes
 */
Register::Register(uint64_t size){

    data = new char[size];

    isNew = false;
}

/**
 * Constructs a Register containing an integer val
 */
Register::Register(int val){

    // set dataType of register
    dataType = Types::Tag::Integer;

    // set to value
    setInteger(val);

    isNew = false;

}

/**
 * get integer, if stored
 * @return: integer of data pointer or -1 if other type stored in register
 */

int Register::getInteger() const{
    
    int ret = -1;
    
    // return value of data pointer cast to integer pointer
    if(dataType == Types::Tag::Integer){
        ret = *((int *) data);
    }
    
    return ret;
}

/**
 * sets integer value of register
 * @param val: integer value to be stored
 */
void Register::setInteger(int val){
    
    if(dataType == Types::Tag::Integer){
        
        // remove old data
        if(!isNew) delete [] data;
        
        // allocate space
        data = new char[sizeof(val)];
        
        // copy integer into memory
        memcpy(data, &val, sizeof(val));
    }
    
}

/**
 * returns the data type of the register
 */
Types::Tag Register::getType() const{
    
    return dataType;
}

/**
 * returns length of stored Char
 * @return: length of Char, 0 if not a Char stored in register
 */
unsigned Register::getCharLength() const{
 
    unsigned ret = 0;
    
    if (dataType == Types::Tag::Char){
        ret = charLen;
    }
    
    return ret;
}


Register::~Register(){
    
    delete [] data;
}
