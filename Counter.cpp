// 计数器模块，根据输入计算各个字符出现的次数。

#include "Counter.h"
#include <fstream>
#include <string.h>
#include <filesystem>

void Counter::writeCountArray(const std::string& filename, uint64_t *countArray) {
    memset(countArray, 0, sizeof(uint64_t) * 256);

    if (std::filesystem::is_directory(filename)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(filename)) {
            if (entry.is_regular_file()) {
                addCountArray(entry.path().string(), countArray);
            }
        }
    } else addCountArray(filename, countArray);
}

void Counter::addCountArray(const std::string& filename, uint64_t *countArray) {
    const size_t bufferSize = 8192; // 缓冲区大小
    char buffer[bufferSize];

    std::ifstream inputFile(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open input file when addCountArray.");
    }

    std::streampos fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    while (fileSize > 0) {
        inputFile.read(buffer, bufferSize);
        std::streamsize readBytes = inputFile.gcount();

        // 卸载缓冲区
        char* curr = buffer;
        // 通过循环展开提高性能
        while (readBytes >= 8) {
            countArray[static_cast<unsigned char>(*curr++)]++;
            countArray[static_cast<unsigned char>(*curr++)]++;
            countArray[static_cast<unsigned char>(*curr++)]++;
            countArray[static_cast<unsigned char>(*curr++)]++;
            countArray[static_cast<unsigned char>(*curr++)]++;
            countArray[static_cast<unsigned char>(*curr++)]++;
            countArray[static_cast<unsigned char>(*curr++)]++;
            countArray[static_cast<unsigned char>(*curr++)]++;
            readBytes -= 8;
        }

        // 处理剩余字符
        while (readBytes > 0) {
            countArray[static_cast<unsigned char>(*curr++)]++;
            --readBytes;
        }

        fileSize -= bufferSize;
    }
    inputFile.close();
}
