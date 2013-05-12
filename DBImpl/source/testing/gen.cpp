/*
 * gen.cpp
 *
 *	Purpose: Generates Testinput-File
 *
 *  @author TUM
 */

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>


namespace testing {

class RandomLong {
	/// The state
	uint64_t state;

public:
	/// Constructor
	explicit RandomLong(uint64_t seed = 88172645463325252ull) :
			state(seed) {
	}

	/// Get the next value
	uint64_t next() {
		state ^= (state << 13);
		state ^= (state >> 7);
		return (state ^= (state << 17));
	}

	static int generateInputFile(const char* dest_file, unsigned n) {

		RandomLong rand;
		if (n == 0) {
			std::cerr << "invalid length: " << n << std::endl;
			return -1;
		}
		int fd, ret;
		if ((fd = open(dest_file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR)) < 0) {
			std::cerr << "cannot open file '" << dest_file << "': " << strerror(errno) << std::endl;
			return -1;
		}
		if ((ret = posix_fallocate(fd, 0, n * sizeof(uint64_t))) != 0)
			std::cerr << "warning: could not allocate file space: " << strerror(ret) << std::endl;
		for (unsigned i = 0; i < n; ++i) {
			uint64_t x = rand.next();
			if (write(fd, &x, sizeof(uint64_t)) < 0) {
				std::cout << "error writing to " << dest_file << ": " << strerror(errno) << std::endl;
			}
		}
		close(fd);
		return 0;
	}
};

}
