#ifndef GUARD_Counter_h
#define GUARD_Counter_h

#include <string>
#include <cstdint>

class Counter {
public:
	static void writeCountArray(const std::string& filename, uint64_t *countArray);
private:
	static void addCountArray(const std::string& filename, uint64_t *countArray);
};

#endif
