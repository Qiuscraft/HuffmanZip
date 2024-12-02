#include "Counter.h"
#include <fstream>
#include <string.h>
#include <filesystem>

void Counter::writeCountArray(const std::string& filename, uint64_t *countArray) {
    memset(countArray, 0, sizeof(uint64_t) * 256);

    addCountArray(filename, countArray);
}

void Counter::writeDirectoryCountArray(const std::string& directoryName, uint64_t *countArray) {
    // Initialize the countArray with zeros
    memset(countArray, 0, sizeof(uint64_t) * 256);
    
    // Use std::filesystem to iterate over files in the directory
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryName)) {
        if (entry.is_regular_file()) {
            addCountArray(entry.path().string(), countArray);
        }
    }
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
