#include "Counter.h"
#include <fstream>
#include <iostream>
#include <array>
#include <string>
#include <filesystem>
#include <stdexcept>

void Counter::writeCountArray(const std::string& path, uint64_t countArray[256]) {
    std::fill_n(countArray, 256, 0);

    if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                addCountArray(entry.path().string(), countArray);
            }
        }
    } else {
        addCountArray(path, countArray);
    }
}

void Counter::addCountArray(const std::string& filename, uint64_t countArray[256]) {
    const size_t bufferSize = 8192; // 缓存大小
    char buffer[bufferSize];

    std::ifstream inputFile(filename, std::ios::in | std::ios::binary);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open input file when adding to count array.");
    }

    while (inputFile) {
        inputFile.read(buffer, bufferSize);
        std::streamsize bytesRead = inputFile.gcount();

        for (std::streamsize i = 0; i < bytesRead; ++i) {
            ++countArray[static_cast<unsigned char>(buffer[i])];
        }
    }
    inputFile.close();
}
