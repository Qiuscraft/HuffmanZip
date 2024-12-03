// 解压缩模块。

#include "HuffmanZip.h"
#include "Counter.h"
#include <fstream>
#include <stdexcept>
#include <bitset>
#include <sstream>
#include "BitOutBuffer.h"
#include "BitInBuffer.h"
#include "XOREncryption.h"

void HuffmanZip::compress(const std::string& inputPath, const std::string& outputPath) {
    compress(inputPath, outputPath, std::string());
}

void HuffmanZip::compress(const std::string& inputPath, const std::string& outputPath, const std::string &password) {
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
        uint32_t emptyDirSize = 0;
        outFile.write((char *) &emptyDirSize, sizeof(emptyDirSize));
        Counter::writeCountArray(inputPath, countArray);
        HuffmanTree tree(countArray);
        writeTree(outFile, tree, password);
        addFileToZip(inputPath, outFile, tree);
    } 
    
    else if (std::filesystem::is_directory(path)) {
        std::string emptyDir;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (is_directory_empty(entry.path())) {
                emptyDir = emptyDir.append(entry.path().string()).append("\n");
            }
        }
        uint32_t emptyDirSize = emptyDir.size();
        outFile.write((char *) &emptyDirSize, sizeof(emptyDirSize));
        outFile.write(emptyDir.c_str(), emptyDirSize);

        Counter::writeDirectoryCountArray(inputPath, countArray);
        HuffmanTree tree(countArray);
        writeTree(outFile, tree, password);
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
    decompress(inputPath, outputPath, std::string());
}

void HuffmanZip::decompress(const std::string& inputPath, const std::string& outputPath, const std::string &password) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) 
        throw std::runtime_error("Failed to open input file");

    // Read empty dir
    uint32_t dirSize;
    inFile.read((char*)&dirSize, sizeof(dirSize));
    std::string dir(dirSize, '\0');
    inFile.read(&dir[0], dirSize);
    createDirectories(dir, outputPath);

    HuffmanTree tree = readTree(inFile, password);

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
    writeTree(outFile, tree, std::string());
}

void HuffmanZip::writeTree(std::ofstream& outFile, const HuffmanTree &tree, const std::string &password) {
    std::string serializedTree = XOREncryption::encryptData(tree.serialize(), password);
    uint32_t treeSize = serializedTree.size();

    // Write tree size and serialized tree to output file
    outFile.write((char *) &treeSize, sizeof(treeSize));
    outFile.write(serializedTree.c_str(), treeSize);
}

HuffmanTree HuffmanZip::readTree(std::ifstream &inFile) {
    return readTree(inFile, std::string());
}

HuffmanTree HuffmanZip::readTree(std::ifstream &inFile, const std::string &password) {
    // Read tree
    uint32_t treeSize;
    inFile.read((char*)&treeSize, sizeof(treeSize));
    std::string serializedTree(treeSize, '\0');
    inFile.read(&serializedTree[0], treeSize);
    serializedTree = XOREncryption::encryptData(serializedTree, password);
    HuffmanTree tree = HuffmanTree(serializedTree);
    return tree;
}

bool HuffmanZip::is_directory_empty(const std::filesystem::path& dirPath) {
    return std::filesystem::is_directory(dirPath) && 
           std::filesystem::begin(std::filesystem::directory_iterator(dirPath)) == std::filesystem::end(std::filesystem::directory_iterator(dirPath));
}

void HuffmanZip::createDirectories(const std::string &paths, const std::string& outputPath) {
    std::istringstream stream(paths);
    std::string path;
    
    while (std::getline(stream, path)) {
        if (!path.empty()) {
            try {
                std::filesystem::path outputFullPath = std::filesystem::path(outputPath) / path;
                std::filesystem::create_directories(outputFullPath);
            } catch (const std::filesystem::filesystem_error &e) {
                std::cerr << "Error creating directory " << path << ": " << e.what() << std::endl;
            }
        }
    }
}
