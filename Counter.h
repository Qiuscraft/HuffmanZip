// 计数器模块，根据输入计算各个字符出现的次数。

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

