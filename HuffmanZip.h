#ifndef GUARD_HuffmanZip_h
#define GUARD_HuffmanZip_h

#include <string>
#include <cstdint>

class HuffmanZip {
public:
    static void compress(const std::string& inputPath, const std::string& outputPath);
    static void decompress(const std::string& inputPath, const std::string& outputPath);
};

#endif
