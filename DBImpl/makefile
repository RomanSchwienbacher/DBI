# Variables
CC=g++
DEBUG=-g -O0 -c
CFLAGS=-std=c++11 -pthread

# All Target
all: dbi

# Tool invocations
dbi: bufman.o mainTest.o sorter.o segment.o
	$(CC) $(CFLAGS) -o "dbi" bufframe.o bufman.o locker.o buffertest.o slottedtest.o bTreeTest.o mainTest.o gtest-all.o sorter.o segment.o si_segment.o fsi_segment.o record.o tid.o sp_segment.o btree_segment.o segman.o slottedpage.o

segment.o: ./source/segments/segment.cpp ./source/segments/si_segment.cpp ./source/segments/fsi_segment.cpp ./source/segments/record.cpp ./source/segments/tid.cpp ./source/segments/sp_segment.cpp ./source/segments/btree_segment.cpp ./source/segments/segman.cpp ./source/segments/slottedpage.cpp
	$(CC) $(DEBUG) $(CFLAGS) ./source/segments/segment.cpp ./source/segments/si_segment.cpp ./source/segments/fsi_segment.cpp ./source/segments/record.cpp ./source/segments/tid.cpp ./source/segments/sp_segment.cpp ./source/segments/btree_segment.cpp ./source/segments/segman.cpp ./source/segments/slottedpage.cpp

bufman.o: ./source/buffer/bufframe.cpp ./source/buffer/bufman.cpp ./source/buffer/locker.cpp
	$(CC) $(DEBUG) $(CFLAGS) ./source/buffer/bufframe.cpp ./source/buffer/bufman.cpp ./source/buffer/locker.cpp

mainTest.o: ./source/testing/buffertest.cpp ./source/testing/slottedtest.cpp ./source/testing/bTreeTest.cpp ./source/testing/mainTest.cpp ./testinglib/gtest/gtest-all.cc
	$(CC) $(DEBUG) $(CFLAGS) ./source/testing/buffertest.cpp ./source/testing/slottedtest.cpp ./source/testing/bTreeTest.cpp ./source/testing/mainTest.cpp ./testinglib/gtest/gtest-all.cc

sorter.o: ./source/sorting/sorter.cpp
	$(CC) $(DEBUG) $(CFLAGS) ./source/sorting/sorter.cpp
	
	
# assignment 3
test3: bufman.o segment.o segtest.o
	$(CC) $(CFLAGS) -o "test3" bufframe.o bufman.o locker.o test3.o gtest-all.o segment.o si_segment.o fsi_segment.o record.o tid.o sp_segment.o segman.o slottedpage.o

segtest.o: ./source/testing/test3/segTest.cpp ./source/testing/test3/test3.cpp ./testinglib/gtest/gtest-all.cc
	$(CC) $(DEBUG) $(CFLAGS) ./source/testing/test3/segTest.cpp ./source/testing/test3/test3.cpp ./testinglib/gtest/gtest-all.cc

# Other Targets
clean:
	rm -rf *o dbi