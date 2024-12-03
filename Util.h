// 调试模块，不涉及业务功能。

#ifndef GUARD_Util_h
#define GUARD_Util_h

#include <string>
#include <cstdint>
#include "HuffmanCode.h"
#include <unordered_map>

namespace Util {
	void printBinary(const std::string &str);
	void writeStringAsBinary(const std::string& inputStr, const std::string& fileName);
	void writeArrayToReadableFile(const uint64_t countArray[256], const std::string& filename);
	void saveHuffmanCodes(const std::unordered_map<char, HuffmanCode>& huffmanMap, const std::string& filename);
	void outputNonZeroValues(const uint64_t arr[256], const std::string &filename);
}

#endif

