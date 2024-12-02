#include "Util.h"
#include <iostream>
#include <bitset>
#include <fstream>
#include <unordered_map>
#include <iomanip>

void Util::printBinary(const std::string &str) {
    for (char c : str) {
        std::bitset<8> bits(c);
        std::cout << bits << " ";
    }
    std::cout << std::endl;
}

void Util::writeStringAsBinary(const std::string& inputStr, const std::string& fileName) {
    // 打开文件以进行写操作
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cerr << "无法打开文件: " << fileName << std::endl;
        return;
    }
    
    // 将每个字符的ASCII值转换为二进制形式，并写入文件
    for (char c : inputStr) {
        // 将字符c转化为8位二进制数表示的字符串
        std::bitset<8> bits(static_cast<unsigned long long>(c));
        
        // 写入二进制字符串形式
        outFile << bits << ' ';
    }
    
    // 关闭文件
    outFile.close();
}

void Util::writeArrayToReadableFile(const uint64_t countArray[256], const std::string& filename) {
    // 打开文件，准备写入
    std::ofstream outFile(filename);
    
    // 检查文件是否成功打开
    if (!outFile) {
        std::cerr << "无法打开文件：" << filename << std::endl;
        return;
    }
    
    // 遍历数组，将每个元素写入文件
    for (int i = 0; i < 256; ++i) {
        // 写入数组元素并添加换行符，使其更易于阅读
        outFile << countArray[i] << std::endl;
    }
    
    // 关闭文件
    outFile.close();
    
    // 检查文件是否成功写入
    if (!outFile.good()) {
        std::cerr << "写入文件时发生错误。" << std::endl;
    } else {
        std::cout << "数据已以人类可读格式成功写入文件：" << filename << std::endl;
    }
}

void Util::saveHuffmanCodes(const std::unordered_map<char, HuffmanCode>& huffmanMap, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& pair : huffmanMap) {
            file << 
            std::hex << std::setw(2) << std::setfill('0') << 
            (unsigned int)(unsigned char)pair.first << 
            std::dec << std::setw(0) << std::setfill(' ') << 
            ": " << pair.second << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Failed to open file for writing." << std::endl;
    }
}

void Util::outputNonZeroValues(const uint64_t arr[256], const std::string &filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error opening the file!" << std::endl;
        return;
    }
    
    for (int i = 0; i < 256; ++i) {
        if (arr[i] != 0) {
            outFile << "Index: " << std::hex << std::setw(2) << std::setfill('0') << i
                    << ", Value: " << std::dec << arr[i] << std::endl;
        }
    }
}
