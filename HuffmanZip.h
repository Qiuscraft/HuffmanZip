#ifndef GUARD_HuffmanZip_h
#define GUARD_HuffmanZip_h

#include <string>
#include <cstdint>
#include "HuffmanTree.h"
#include <filesystem>

class HuffmanZip {
public:
    static void compress(const std::string& inputPath, const std::string& outputPath);
    static void decompress(const std::string& inputPath, const std::string& outputPath);
    static bool is_directory_empty(const std::filesystem::path& dirPath);
private:
    static void addFileToZip(const std::string& inputPath, std::ofstream& outFile, const HuffmanTree &tree);
    static void writeTree(std::ofstream& outFile, const HuffmanTree &tree);
    static void createDirectories(const std::string &paths, const std::string& outputPath);
};

#endif
