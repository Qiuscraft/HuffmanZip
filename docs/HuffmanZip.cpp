#include "HuffmanZip.h"
#include "Counter.h"
#include "HuffmanTree.h"
#include <fstream>
#include <stdexcept>
#include <bitset>
#include <sstream>
#include "BitBuffer.h"

void HuffmanZip::compress(const std::string& inputPath, const std::string& outputPath) 
{
    // Open file in binary mode
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open input file");
    }
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open output file");
    }

    // Build Huffman tree and serialize it
    std::map<char, int> countMap = Counter::getCountMap(inputPath);
    HuffmanTree tree(countMap);
    std::string serializedTree = tree.serialize();
    uint32_t treeSize = serializedTree.size();

    // Write tree size and serialized tree to output file
    outFile.write(reinterpret_cast<char*>(&treeSize), sizeof(treeSize));
    outFile.write(serializedTree.c_str(), treeSize);

    // Reserve space for amount_of_byte and padding_size
    uint32_t placeholder = 0;
    outFile.write(reinterpret_cast<char*>(&placeholder), sizeof(placeholder)); // amount_of_byte
    outFile.write(reinterpret_cast<char*>(&placeholder), sizeof(placeholder)); // padding_size

    // prepare codeMap and BitOutBuffer for compression
    std::map<char, HuffmanCode> codeMap = tree.getCodeMap();
    char c;
    uint64_t amount_of_byte = 0;
    uint32_t padding_size = 0;

    // Read input file and write Huffman codes to buffer and output file
    BitOutBuffer buffer(bufferSize);
    while (inFile.get(c)) {
        try {
            buffer.writeCode(codeMap[c]);
        } catch(...) {
            amount_of_byte += buffer.writeToFile(outFile);
            buffer.writeCode(codeMap[c]);
        }
    }
    amount_of_byte += buffer.writeToFileWithZero(outFile, padding_size);

    outFile.seekp(sizeof(treeSize) + treeSize, std::ios::beg);
    if (!outFile) {
        throw std::runtime_error("Failed to seek to the reserved position");
    }
    outFile.write(reinterpret_cast<char*>(&amount_of_byte), sizeof(amount_of_byte));
    outFile.write(reinterpret_cast<char*>(&padding_size), sizeof(padding_size));

    // Close the files
    inFile.close();
    outFile.close();
}

void HuffmanZip::decompress(const std::string& inputPath, const std::string& outputPath) 
{
    // Open input file in binary mode
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open input file");
    }
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open output file");
    }

    // Read tree
    uint32_t networkTreeSize = 0;
    inFile.read(reinterpret_cast<char*>(&networkTreeSize), sizeof(networkTreeSize));
    if (inFile.gcount() != sizeof(networkTreeSize)) {
        throw std::runtime_error("Failed to read tree size");
    }
    uint32_t treeSize = ntohl(networkTreeSize);
    std::string serializedTree(treeSize, '\0');
    inFile.read(&serializedTree[0], treeSize);
    if (static_cast<uint32_t>(inFile.gcount()) != treeSize) {
        throw std::runtime_error("Failed to read serialized Huffman tree");
    }
    HuffmanTree tree = HuffmanTree(serializedTree);

    // Read amount_of_byte and padding_size
    uint32_t networkAmount = 0;
    uint32_t networkPadding = 0;
    inFile.read(reinterpret_cast<char*>(&networkAmount), sizeof(networkAmount));
    if (inFile.gcount() != sizeof(networkAmount)) {
        throw std::runtime_error("Failed to read amount_of_byte");
    }
    inFile.read(reinterpret_cast<char*>(&networkPadding), sizeof(networkPadding));
    if (inFile.gcount() != sizeof(networkPadding)) {
        throw std::runtime_error("Failed to read padding_size");
    }
    uint64_t amount_of_byte = ntohl(networkAmount);
    uint32_t padding_size = ntohl(networkPadding);

    // TODO: 分块多次进入buffer
    // Initialize BitOutBuffer with the amount_of_byte
    uint64_t bufferSize = amount_of_byte * 8;
    BitOutBuffer buffer(bufferSize);
    // TODO: 进入buffer

    // Initialize variables for decoding
    HuffmanNode* currentNode = tree.root;
    char decodedChar;
    uint64_t totalBytesWritten = 0;

    // Decode the bitstream
    while (totalBytesWritten < amount_of_byte) {
        bool bit;
        try {
            bool bit = buffer.outBit();
            if (bit) {
                currentNode = currentNode->rightChild;
            } else {
                currentNode = currentNode->leftChild;
            }

            if (currentNode->data != '\0') {
                decodedChar = currentNode->data;
                outFile.put(decodedChar);
                if (!outFile) {
                    throw std::runtime_error("Failed to write to output file");
                }
                totalBytesWritten++;
                currentNode = tree.root;
            }
        } catch (...) {

        }
    }

    // Handle padding if any
    if (padding_size > 0) {
        // Optionally verify that the padding bits are zero
        for (uint32_t i = 0; i < padding_size; ++i) {
            bool bit;
            if (buffer.getNextBit(bit)) {
                if (bit) {
                    throw std::runtime_error("Padding bits are not zero");
                }
            }
        }
    }

    // Close the files
    inFile.close();
    outFile.close();
}

uint64_t HuffmanZip::totalBitSizeafterCompression(
	const std::map<char, int> &countMap, const std::map<char, HuffmanCode> &codeMap) {
	uint64_t bitSize = 0;
	for (const auto& [character, count] : countMap) {
		bitSize += count * codeMap.at(character).getSize();
	}
	return bitSize;
}
