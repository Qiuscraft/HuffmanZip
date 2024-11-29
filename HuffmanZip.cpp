#include "HuffmanZip.h"
#include "Counter.h"
#include "HuffmanTree.h"
#include <fstream>
#include <stdexcept>
#include <bitset>
#include <sstream>
#include "BitOutBuffer.h"
#include "BitInBuffer.h"
#include "Util.h"

#include <iostream>

void HuffmanZip::compress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) 
        throw std::runtime_error("Failed to open input file");

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) 
        throw std::runtime_error("Failed to open output file");

    // Build Huffman tree
    uint64_t countArray[256];
    Counter::writeCountArray(inputPath, countArray);
    HuffmanTree tree(countArray);
    std::string serializedTree = tree.serialize();
    uint32_t treeSize = serializedTree.size();

    // Write tree size and serialized tree to output file
    outFile.write((char *) &treeSize, sizeof(treeSize));
    outFile.write(serializedTree.c_str(), treeSize);

    // Reserve space for bitAmount and paddingBitAmount
    uint64_t bitAmount = 0;
    uint32_t paddingBitAmount = 0;
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

    outFile.seekp(sizeof(treeSize) + treeSize, std::ios::beg);
    if (!outFile) 
        throw std::runtime_error("Failed to seek to the reserved position");
    
    outFile.write((char *) &bitAmount, sizeof(bitAmount));
    outFile.write((char *) &paddingBitAmount, sizeof(paddingBitAmount));

    inFile.close();
    outFile.close();
}

void HuffmanZip::decompress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) 
        throw std::runtime_error("Failed to open input file");
    
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) 
        throw std::runtime_error("Failed to open output file");

    // Read tree
    uint32_t treeSize;
    inFile.read((char*)&treeSize, sizeof(treeSize));
    std::string serializedTree(treeSize, '\0');
    inFile.read(&serializedTree[0], treeSize);
    HuffmanTree tree = HuffmanTree(serializedTree);

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

    inFile.close();
    outFile.close();
}
