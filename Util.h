#ifndef GUARD_Util_h
#define GUARD_Util_h

#include <string>
#include <cstdint>
#include "HuffmanCode.h"
#include <unordered_map>

class Util {
public:
	static void printBinary(const std::string &str);
	static void writeStringAsBinary(const std::string& inputStr, const std::string& fileName);
	static void writeArrayToReadableFile(const uint64_t countArray[256], const std::string& filename);
	static void saveHuffmanCodes(const std::unordered_map<char, HuffmanCode>& huffmanMap, const std::string& filename);
};

#endif

