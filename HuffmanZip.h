#ifndef GUARD_HuffmanZip_h
#define GUARD_HuffmanZip_h

#include <string>
#include <cstdint>
#include "HuffmanTree.h"
#include <filesystem>

class HuffmanZip {
public:
    static void compress(const std::string& inputPath, const std::string& outputPath, 
        const std::string &password, std::istream &istream, std::ostream &ostream, bool overwrite = false);

    static void decompress(const std::string& input, const std::string& output, 
        const std::string &password, std::istream &istream, std::ostream &ostream, bool overwrite = false);
private:
    static void writeEmptyDirectories(std::ofstream &outFile, const std::string& inputPath);

    static bool decompressFile(std::ifstream &inFile, const std::string& outputPath, const HuffmanTree &tree, std::istream &istream, std::ostream &ostream, bool overwrite = false);
    static void decodeContent(std::ifstream &inFile, std::ofstream &outFile, const HuffmanTree &tree, uint64_t actualBits);

    static void addFileToZip(const std::string& inputPath, std::ofstream& outFile, const HuffmanTree &tree);
    static void writeCompressedData(std::ofstream &outFile, const std::unordered_map<char, HuffmanCode>& codeMap, uint64_t &bitAmount, uint32_t &paddingBitAmount, const std::string &inputPath);
    
    static void writeTree(std::ofstream& outFile, const HuffmanTree &tree, const std::string &password);
    static HuffmanTree readTree(std::ifstream &inFile, const std::string &password);
    static void createDirectories(const std::string &paths, const std::string& outputPath);
    static void readCreateEmptyDirectories(std::ifstream &inFile, const std::string &outputPath);

    static bool is_directory_empty(const std::filesystem::path& dirPath);
};

#endif
