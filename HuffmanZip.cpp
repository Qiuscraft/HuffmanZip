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

    std::filesystem::path path(inputPath);

    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Failed to open input file");
    }

    if (std::filesystem::is_regular_file(path)) {
        handleFileCompression(inputPath, outFile, password);
    } 
    else if (std::filesystem::is_directory(path)) {
        handleDirectoryCompression(inputPath, outFile, password);
    } 
    else {
        throw std::runtime_error("The inputPath is neither a regular file nor a directory.");
    }
    outFile.close();
}

void HuffmanZip::handleFileCompression(const std::string& inputPath, std::ofstream &outFile, const std::string &password) {
    // 向压缩包写入空文件夹数据
    uint32_t emptyDirSize = 0;
    outFile.write((char *) &emptyDirSize, sizeof(emptyDirSize));
    // 向压缩包写入树数据
    uint64_t countArray[256];
    Counter::writeCountArray(inputPath, countArray);
    HuffmanTree tree(countArray);
    writeTree(outFile, tree, password);
    // 向压缩包写入每个文件的数据
    addFileToZip(inputPath, outFile, tree);
}

void HuffmanZip::handleDirectoryCompression(const std::string& inputPath, std::ofstream &outFile, const std::string &password) {
    // 向压缩包写入空文件夹数据
    writeEmptyDirectories(outFile, inputPath);
    // 向压缩包写入树数据
    uint64_t countArray[256];
    Counter::writeDirectoryCountArray(inputPath, countArray);
    HuffmanTree tree(countArray);
    writeTree(outFile, tree, password);
    // 向压缩包写入每个文件的数据
    for (const auto& entry : std::filesystem::recursive_directory_iterator(inputPath)) {
        if (std::filesystem::is_regular_file(entry.path())) {
            addFileToZip(entry.path().string(), outFile, tree);
        }
    }
}

// 向压缩包中写入空文件夹数据
void HuffmanZip::writeEmptyDirectories(std::ofstream &outFile, const std::string& inputPath) {
    std::string emptyDir;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(inputPath)) {
        if (is_directory_empty(entry.path())) {
            emptyDir = emptyDir.append(entry.path().string()).append("\n");
        }
    }
    uint32_t emptyDirSize = emptyDir.size();
    outFile.write((char *) &emptyDirSize, sizeof(emptyDirSize));
    outFile.write(emptyDir.c_str(), emptyDirSize);
}

void HuffmanZip::decompress(const std::string& inputPath, const std::string& outputPath) {
    decompress(inputPath, outputPath, std::string());
}

void HuffmanZip::decompress(const std::string& inputPath, const std::string& outputPath, const std::string &password) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) 
        throw std::runtime_error("Failed to open input file");

    // 读取空文件夹数据并创建空文件夹
    uint32_t dirSize;
    inFile.read((char*)&dirSize, sizeof(dirSize));
    std::string dir(dirSize, '\0');
    inFile.read(&dir[0], dirSize);
    createDirectories(dir, outputPath);

    // 读取树
    HuffmanTree tree = readTree(inFile, password);

    // 解压每一个文件。
    while (true) {
        if (!decompressFile(inFile, outputPath, tree)) {
            break;
        }
    }

    inFile.close();
}

// 解压文件，解压成功返回true，当走到压缩包末尾，没有文件可以解压时返回false。
bool HuffmanZip::decompressFile(std::ifstream &inFile, const std::string& outputPath, const HuffmanTree &tree) {
    uint32_t filenameSize;
    inFile.read((char*)&filenameSize, sizeof(filenameSize));

    if (inFile.eof())
        return false;
    
    // 读取文件名
    std::string filename(filenameSize, '\0');
    inFile.read(&filename[0], filenameSize);

    // 读取并创建文件所在的文件夹
    std::filesystem::path outputFullPath = std::filesystem::path(outputPath) / filename;
    std::filesystem::create_directories(outputFullPath.parent_path());

    std::ofstream outFile(outputFullPath, std::ios::binary);
    if (!outFile.is_open()) 
        throw std::runtime_error("Failed to open output file");

    // 读取文件实际长度
    uint64_t bitAmount;
    uint32_t paddingBitAmount;
    inFile.read((char*)&bitAmount, sizeof(bitAmount));
    inFile.read((char*)&paddingBitAmount, sizeof(paddingBitAmount));

    uint64_t actualBits = bitAmount - paddingBitAmount;

    decodeContent(inFile, outFile, tree, actualBits);
    outFile.close();
    return true;
}

void HuffmanZip::decodeContent(std::ifstream &inFile, std::ofstream &outFile, const HuffmanTree &tree, uint64_t actualBits) {
    BitInBuffer buffer(inFile);
    HuffmanNode* currentNode = tree.getRoot();

    for (uint64_t i = 0; i < actualBits; ++i) {
        bool bit = buffer.readBit();
        currentNode = bit ? currentNode->rightChild : currentNode->leftChild;

        if (currentNode->isLeaf()) {
            outFile.put(currentNode->getCharacter());
            currentNode = tree.getRoot();
        }
    }
}

void HuffmanZip::addFileToZip(const std::string& inputPath, std::ofstream& outFile, const HuffmanTree &tree) {
    uint32_t filenameSize = inputPath.size();
    outFile.write((char *) &filenameSize, sizeof(filenameSize));
    outFile.write(inputPath.c_str(), filenameSize);

    // 预留位置以便回写 bitAmount 和 paddingBitAmount
    uint64_t bitAmount = 0;
    uint32_t paddingBitAmount = 0;
    std::streampos bitAmountPos = outFile.tellp();
    outFile.write((char *) &bitAmount, sizeof(bitAmount)); // bitAmount
    outFile.write((char *) &paddingBitAmount, sizeof(paddingBitAmount)); // paddingBitAmount

    // 压缩数据并计算比特数量
    writeCompressedData(outFile, tree.getCodeMap(), bitAmount, paddingBitAmount, inputPath);

    // 移动到预留位置并写入实际值
    outFile.seekp(bitAmountPos);
    if (!outFile) 
        throw std::runtime_error("Failed to seek to the reserved position in file");
    outFile.write((char *) &bitAmount, sizeof(bitAmount));
    outFile.write((char *) &paddingBitAmount, sizeof(paddingBitAmount));

    // 移动到文件末尾
    outFile.seekp(0, std::ios::end); 
    if (!outFile) 
        throw std::runtime_error("Failed to seek to the end of file");
}

// 压缩数据并计算比特数量
void HuffmanZip::writeCompressedData(std::ofstream &outFile, const std::unordered_map<char, HuffmanCode>& codeMap, uint64_t &bitAmount, uint32_t &paddingBitAmount, const std::string &inputPath) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) 
        throw std::runtime_error("Failed to open input file");

    char c;
    BitOutBuffer buffer(outFile);
    while (inFile.get(c)) {
        buffer.write(codeMap.at(c));
        bitAmount += codeMap.at(c).getSize();
    }

    paddingBitAmount = buffer.finalWriteWithPaddings();
    bitAmount += paddingBitAmount;

    inFile.close();
}

void HuffmanZip::writeTree(std::ofstream& outFile, const HuffmanTree &tree) {
    writeTree(outFile, tree, std::string());
}

void HuffmanZip::writeTree(std::ofstream& outFile, const HuffmanTree &tree, const std::string &password) {
    std::string serializedTree = XOREncryption::encryptData(tree.serialize(), password);
    uint32_t treeSize = serializedTree.size();

    outFile.write((char *) &treeSize, sizeof(treeSize));
    outFile.write(serializedTree.c_str(), treeSize);
}

HuffmanTree HuffmanZip::readTree(std::ifstream &inFile) {
    return readTree(inFile, std::string());
}

HuffmanTree HuffmanZip::readTree(std::ifstream &inFile, const std::string &password) {
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
