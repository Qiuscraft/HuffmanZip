#include "HuffmanZip.h"
#include "Counter.h"
#include <fstream>
#include <stdexcept>
#include <bitset>
#include <sstream>
#include "BitOutBuffer.h"
#include "BitInBuffer.h"
#include <filesystem>

// TODO: 压缩成功后文件的大端序小端序。

void HuffmanZip::compress(const std::string& inputPath, const std::string& outputPath) {
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) 
        throw std::runtime_error("Failed to open output file");

    // Build Huffman tree
    uint64_t countArray[256];

    std::filesystem::path path(inputPath);

    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Failed to open input file");
    } 
    
    else if (std::filesystem::is_regular_file(path)) {
        Counter::writeCountArray(inputPath, countArray);
        HuffmanTree tree(countArray);
        writeTree(outFile, tree);
        addFileToZip(inputPath, outFile, tree);
    } 
    
    else if (std::filesystem::is_directory(path)) {
        Counter::writeDirectoryCountArray(inputPath, countArray);
        HuffmanTree tree(countArray);
        writeTree(outFile, tree);
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                addFileToZip(entry.path().string(), outFile, tree);
            }
        }
    } 
    
    else {
        throw std::runtime_error("The inputPath is neither a regular file nor a directory.");
    }
    outFile.close();
}

void HuffmanZip::decompress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) 
        throw std::runtime_error("Failed to open input file");

    // Read tree
    uint32_t treeSize;
    inFile.read((char*)&treeSize, sizeof(treeSize));
    std::string serializedTree(treeSize, '\0');
    inFile.read(&serializedTree[0], treeSize);
    HuffmanTree tree = HuffmanTree(serializedTree);

    while (true) {
        // Read OutFile
        uint32_t filenameSize;
        inFile.read((char*)&filenameSize, sizeof(filenameSize));

        if (inFile.eof())
            break;
        
        std::string filename(filenameSize, '\0');
        inFile.read(&filename[0], filenameSize);

        std::filesystem::path outputFullPath = std::filesystem::path(outputPath) / filename;

        // Ensure that the directory exists
        std::filesystem::create_directories(outputFullPath.parent_path());

        std::ofstream outFile(outputFullPath, std::ios::binary);
        if (!outFile.is_open()) 
            throw std::runtime_error("Failed to open output file");

        // Read bitAmount and paddingBitAmount
        uint64_t bitAmount;
        uint32_t paddingBitAmount;
        inFile.read((char*)&bitAmount, sizeof(bitAmount));
        inFile.read((char*)&paddingBitAmount, sizeof(paddingBitAmount));

        uint64_t actualBits = bitAmount - paddingBitAmount;

        // Read compressed data and decode
        BitInBuffer buffer(inFile);
        HuffmanNode* currentNode = tree.getRoot();

        for (uint64_t i = 0; i < actualBits; ++i) {
            bool bit = buffer.readBit();
            
            if (bit) {
                currentNode = currentNode->rightChild;
            } else {
                currentNode = currentNode->leftChild;
            }

            if (currentNode->isLeaf()) {
                outFile.put(currentNode->getCharacter());
                currentNode = tree.getRoot();
            }
        }

        outFile.close();
    }

    inFile.close();
}

void HuffmanZip::addFileToZip(const std::string& inputPath, std::ofstream& outFile, const HuffmanTree &tree) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) 
        throw std::runtime_error("Failed to open input file");

    uint32_t filenameSize = inputPath.size();
    outFile.write((char *) &filenameSize, sizeof(filenameSize));
    outFile.write(inputPath.c_str(), filenameSize);

    // Reserve space for bitAmount and paddingBitAmount
    uint64_t bitAmount = 0;
    uint32_t paddingBitAmount = 0;
    
    // Record the current position to write bitAmount and paddingBitAmount later
    std::streampos bitAmountPos = outFile.tellp();
    outFile.write((char *) &bitAmount, sizeof(bitAmount)); // bitAmount
    outFile.write((char *) &paddingBitAmount, sizeof(paddingBitAmount)); // paddingBitAmount

    // Read input file and write Huffman codes to output file
    std::unordered_map<char, HuffmanCode> codeMap = tree.getCodeMap();
    char c;

    BitOutBuffer buffer(outFile);
    while (inFile.get(c)) {
        buffer.write(codeMap[c]);
        bitAmount += codeMap[c].getSize();
    }

    paddingBitAmount = buffer.finalWriteWithPaddings();
    bitAmount += paddingBitAmount;

    // Move to the recorded position and write the actual values
    outFile.seekp(bitAmountPos);
    if (!outFile) 
        throw std::runtime_error("Failed to seek to the reserved position in file");

    outFile.write((char *) &bitAmount, sizeof(bitAmount));
    outFile.write((char *) &paddingBitAmount, sizeof(paddingBitAmount));

    // Move to the end of the file
    outFile.seekp(0, std::ios::end); 
    if (!outFile) 
        throw std::runtime_error("Failed to seek to the end of file");

    inFile.close();
}


void HuffmanZip::writeTree(std::ofstream& outFile, const HuffmanTree &tree) {
    std::string serializedTree = tree.serialize();
    uint32_t treeSize = serializedTree.size();

    // Write tree size and serialized tree to output file
    outFile.write((char *) &treeSize, sizeof(treeSize));
    outFile.write(serializedTree.c_str(), treeSize);
}
