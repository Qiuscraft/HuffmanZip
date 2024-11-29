#ifndef GUARD_HuffmanZip_h
#define GUARD_HuffmanZip_h

#include <string>
#include <cstdint>

class HuffmanZip {
private:
    static uint64_t totalBitSizeafterCompression(
		const std::map<char, int> &countMap, const std::map<char, HuffmanCode> &codeMap);
public:
    static void compress(const std::string& inputPath, const std::string& outputPath);
    static void decompress(const std::string& inputPath, const std::string& outputPath);
};

#endif
