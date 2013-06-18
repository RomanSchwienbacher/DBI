# Variables
CC=g++
DEBUG=-g -O0 -c
CFLAGS=-std=c++11 -pthread

#Source files
SRCBUF=./source/buffer/
SRCIND=./source/index/
SRCSEG=./source/segments/
SRCSORT=./source/sorting/
SRCTEST=./source/testing/
SRCLIB=./testinglib/gtest/
SRCPARSELIB=./parsinglib/

#Object Files
OBJ= ./obj/segments/btree_segment.o ./obj/testing/bTreeTest.o ./obj/testing/buffertest.o ./obj/buffer/bufframe.o ./obj/buffer/bufman.o ./obj/segments/fsi_segment.o ./obj/testinglib/gtest-all.o ./obj/parsinglib/Parser.o ./obj/parsinglib/Schema.o ./obj/buffer/locker.o ./obj/testing/mainTest.o ./obj/segments/record.o ./obj/segments/segman.o ./obj/segments/segment.o ./obj/segments/si_segment.o ./obj/segments/slottedpage.o ./obj/sorting/sorter.o ./obj/segments/sp_segment.o ./obj/segments/tid.o ./obj/segments/sch_segment.o

# Info
# $@ is the name of the target
# $? is the list of dependents more recent than the target

# All Target
all: dbi

# Tool invocations
dbi: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

./obj/buffer/%.o: $(SRCBUF)%.cpp
	$(CC) $(DEBUG) $(CFLAGS) $? -o $@

./obj/index/%.o: $(SRCIND)%.cpp
	$(CC) $(DEBUG) $(CFLAGS) $? -o $@

./obj/segments/%.o: $(SRCSEG)%.cpp
	$(CC) $(DEBUG) $(CFLAGS) $? -o $@

./obj/sorting/%.o: $(SRCSORT)%.cpp
	$(CC) $(DEBUG) $(CFLAGS) $? -o $@

./obj/testing/%.o: $(SRCTEST)%.cpp
	$(CC) $(DEBUG) $(CFLAGS) $? -o $@

./obj/testinglib/%.o: $(SRCLIB)%.cc
	$(CC) $(DEBUG) $(CFLAGS) $? -o $@

./obj/parsinglib/%.o: $(SRCPARSELIB)%.cpp
	$(CC) $(DEBUG) $(CFLAGS) $? -o $@	

# Other Targets
clean:
	rm -rf *o dbi test3 ./obj; mkdir ./obj ./obj/buffer ./obj/index ./obj/segments ./obj/sorting ./obj/testing ./obj/testinglib ./obj/parsinglib ./obj/parsinglib
