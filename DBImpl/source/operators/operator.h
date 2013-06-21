//
//  Operator.h
//  Operator
//
//  Created by David Grotzky on 6/20/13.
//  Copyright (c) 2013 David Grotzky. All rights reserved.
//

#ifndef Operator_Operator_h
#define Operator_Operator_h

#include <vector>
#include "register.h"

using namespace std;

class Operator{
    
public:
	Operator();
	
	void open();
	
	bool next();
    
	vector<Register*> getOutput();
	
	void close();
    
    ~Operator();
};


#endif
