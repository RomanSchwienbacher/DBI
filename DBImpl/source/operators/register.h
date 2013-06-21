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
#include "../../parsinglib/Types.hpp"


class Register{
    
    // data pointer
    char* data;
    
    // data type vector
    Types::Tag dataType;
    
    // char length - if char stored
    unsigned charLen;
    
    // boolean for initialization
    bool isNew = true;
    
    // 
    
public:
    
    // Constructors
    Register(uint64_t size);
    
    Register(int val);
    
    template <unsigned len>
    Register(const Char<len> val){
        // set dataType of register
        dataType = Types::Tag::Char;
        
        // set to value
        setChar(val);
        
        isNew = false;

    }
    
    // Comparators
    template <unsigned len>
    friend bool operator< (const Register &reg1, const Register &reg2){

        bool ret  = false;

        // compare integers
        if (reg1.getType() == Types::Tag::Integer){
            ret = reg1.getInteger() < reg2.getInteger();
        }
        // compare chars
        else if(reg1.getType() == Types::Tag::Char){
            ret = memcmp(reg1.getChar<len>().data, reg2.getChar<len>().data, len) < 0;
        }
        // if one is integer and the other Char
        else if(reg1.getType() != reg2.getType()){
            ret = reg1.getType() == Types::Tag::Integer;
        }
    }
    friend bool operator== (Register &reg1, Register &reg2);
    
    // Getters
    int getInteger() const;
    
    template <unsigned len>
    Char<len> getChar(){
        Char<len>* ret = NULL;
        
        // data pointer cast to Char<len>
        if(dataType == Types::Tag::Char){
            ret = (Char<len> *) data;
        }
        
        return *ret;
    }
    
    Types::Tag getType() const;
    
    unsigned getCharLength() const;
    
    // Setters
    void setInteger(int val);
    
    template <unsigned len>
    void setChar(const Char<len> val){
        if(dataType == Types::Tag::Char){
            
            // remove old data - if applicable
            if(!isNew) delete [] data;
            
            // set char length
            charLen = len;
            
            // allocate space
            data = new char[sizeof(Char<len>)];
            
            // copy integer into memory
            memcpy(data, &val, sizeof(Char<len>));
        }
    }
    
    // Destructor
    ~Register();
    
    
};

#endif /* defined(__Operator__Register__) */
