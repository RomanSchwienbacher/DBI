# All Target
all: DBI

# Tool invocations
DBI: $(OBJS) $(USER_OBJS)
	g++ -std=c++11 -pthread -o "dbi" ./buffer/bufframe.cpp ./buffer/bufman.cpp ./buffer/locker.cpp ./sorting/sorter.cpp ./testing/buffertest.cpp ./testing/mainTest.cpp ./gtest/gtest-all.cc

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) DBI
	-@echo ' '