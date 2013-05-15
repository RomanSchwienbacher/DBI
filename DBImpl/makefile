# All Target
all: DBI

# Tool invocations
DBI: $(OBJS) $(USER_OBJS)
	g++ -std=c++11 -pthread -g -O0 -o "dbi" ./source/buffer/bufframe.cpp ./source/buffer/bufman.cpp ./source/buffer/locker.cpp ./source/sorting/sorter.cpp ./source/testing/buffertest.cpp ./source/testing/mainTest.cpp ./testinglib/gtest/gtest-all.cc

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) DBI
	-@echo ' '