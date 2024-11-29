#ifndef GUARD_Counter_h
#define GUARD_Counter_h

#include <string>
#include <cstdint>

class Counter {
public:
	static void writeCountArray(const std::string& filename, uint64_t *countArray);
	static void writeCountArray(const std::string& filename, uint64_t *countArray, size_t bufferSize);
};

#endif

