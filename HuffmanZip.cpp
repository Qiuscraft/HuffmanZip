#include "HuffmanZip.h"
#include "Counter.h"
#include <fstream>
#include <stdexcept>
#include <bitset>
#include <sstream>
#include "BitOutBuffer.h"
#include "BitInBuffer.h"
#include "XOREncryption.h"

void HuffmanZip::compress(const std::string& inputPath, const std::string& outputPath, 
    const std::string &password, std::istream &istream, std::ostream &ostream, bool overwrite) {

    std::filesystem::path path(inputPath);
    if (!std::filesystem::exists(path)) 
        throw std::runtime_error("Input file does not exist.");
    
    if (!overwrite && std::filesystem::exists(outputPath)) {
        ostream << "\033[32m" << outputPath << "\033[0m" << " already exists, overwrite it? Y/[N]" << std::endl;
        char c;
        istream.get(c);
        if (c != 'y' && c != 'Y') {
            ostream << "Abort compression." << std::endl;
            return;
        }
        ostream << "Overwrite it." << std::endl;
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) 
        throw std::runtime_error("Failed to open output file");

    outFile.put(127); // 先写一个char(127)，标志着该文件由本程序生成。
    writeEmptyDirectories(outFile, inputPath); // 向压缩包写入空文件夹数据

    // 向压缩包写入树数据
    uint64_t countArray[256];
    Counter::writeCountArray(inputPath, countArray);
    HuffmanTree tree(countArray);
    writeTree(outFile, tree, password);

    if (std::filesystem::is_regular_file(path)) 
        addFileToZip(inputPath, outFile, tree);
    else if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(inputPath)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                addFileToZip(entry.path().string(), outFile, tree);
            }
        }
    }
    
    outFile.close();
}

void HuffmanZip::decompress(const std::string& input, const std::string& output,
        const std::string &password, std::istream &istream, std::ostream &ostream, bool overwrite) {

    std::ifstream inFile(input, std::ios::binary);
    if (!inFile.is_open()) 
        throw std::runtime_error("Failed to open input file");

    // 读取标志位，检查是否由本程序生成。
    char c;
    inFile.get(c);
    if (c != 127)
        throw std::domain_error("This file is not generated by huffmanzip.");

    readCreateEmptyDirectories(inFile, output); // 读取空文件夹数据并创建空文件夹
    HuffmanTree tree = readTree(inFile, password); // 读取树

    // 解压每一个文件。
    while (true) {
        if (!decompressFile(inFile, output, tree, istream, ostream, overwrite)) {
            break;
        }
    }

    inFile.close();
}

// 向压缩包中写入空文件夹数据
void HuffmanZip::writeEmptyDirectories(std::ofstream &outFile, const std::string& inputPath) {
    std::string emptyDir;
    if (std::filesystem::is_directory(inputPath)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(inputPath)) {
            if (is_directory_empty(entry.path())) {
                emptyDir = emptyDir.append(entry.path().string()).append("\n");
            }
        }
    }
    uint32_t emptyDirSize = emptyDir.size();
    outFile.write((char *) &emptyDirSize, sizeof(emptyDirSize));
    outFile.write(emptyDir.c_str(), emptyDirSize);
}

// 解压文件。当走到压缩包末尾，返回false，其他情况返回true。
bool HuffmanZip::decompressFile(std::ifstream &inFile, const std::string& outputPath, 
        const HuffmanTree &tree, std::istream &istream, std::ostream &ostream, bool overwrite) {

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

    // 读取文件实际长度
    uint64_t bitAmount;
    inFile.read((char*)&bitAmount, sizeof(bitAmount));

    // 询问用户是否覆盖文件
    if (!overwrite && std::filesystem::exists(outputFullPath)) {
        ostream << "\033[32m" << outputFullPath.lexically_normal().string() << "\033[0m" << " already exists, overwrite it? Y/[N]" << std::endl;
        char c;
        istream.get(c);
        if (c != 'y' && c != 'Y') {
            ostream << "Skipped. " << std::endl;
            inFile.seekg(bitAmount / 8 + sizeof(uint32_t), std::ios::cur);
            return true;
        }
        ostream << "Overwrite it. " << std::endl;
    }

    uint32_t paddingBitAmount;
    inFile.read((char*)&paddingBitAmount, sizeof(paddingBitAmount));
    uint64_t actualBits = bitAmount - paddingBitAmount;

    std::ofstream outFile(outputFullPath, std::ios::binary);
    if (!outFile.is_open()) 
        throw std::runtime_error("Failed to open output file");

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

void HuffmanZip::writeTree(std::ofstream& outFile, const HuffmanTree &tree, const std::string &password) {
    std::string serializedTree = XOREncryption::encryptData(tree.serialize(), password);
    uint32_t treeSize = serializedTree.size();

    outFile.write((char *) &treeSize, sizeof(treeSize));
    outFile.write(serializedTree.c_str(), treeSize);
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

// 用于提前创建文件夹，否则解压缩时无法写入文件。
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

// 读取空文件夹数据并创建空文件夹
void HuffmanZip::readCreateEmptyDirectories(std::ifstream &inFile, const std::string &outputPath) {
    uint32_t dirSize;
    inFile.read((char*)&dirSize, sizeof(dirSize));
    std::string dir(dirSize, '\0');
    inFile.read(&dir[0], dirSize);
    createDirectories(dir, outputPath);
}
