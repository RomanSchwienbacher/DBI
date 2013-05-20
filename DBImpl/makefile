# Variables
CC=g++
CFLAGS=-std=c++11 -pthread -c -g -O0

# All Target
all: dbi

# Tool invocations
dbi: buffer testing sorting segments
	$(CC) -std=c++11 -pthread -o "dbi" bufframe.o bufman.o locker.o buffertest.o mainTest.o gtest-all.o sorter.o segment.o si_segment.o fsi_segment.o 

segments: ./source/segments/segment.cpp ./source/segments/si_segment.cpp ./source/segments/fsi_segment.cpp
	$(CC) $(CFLAGS) ./source/segments/segment.cpp ./source/segments/si_segment.cpp ./source/segments/fsi_segment.cpp 

buffer: ./source/buffer/bufframe.cpp ./source/buffer/bufman.cpp ./source/buffer/locker.cpp
	$(CC) $(CFLAGS) ./source/buffer/bufframe.cpp ./source/buffer/bufman.cpp ./source/buffer/locker.cpp

testing: ./source/testing/buffertest.cpp ./source/testing/mainTest.cpp ./testinglib/gtest/gtest-all.cc
	$(CC) $(CFLAGS) ./source/testing/buffertest.cpp ./source/testing/mainTest.cpp ./testinglib/gtest/gtest-all.cc

sorting: ./source/sorting/sorter.cpp
	$(CC) $(CFLAGS) ./source/sorting/sorter.cpp

# Other Targets
clean:
	rm -rf *o dbi