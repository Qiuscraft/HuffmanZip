#include "Counter.h"
#include <fstream>
#include <iostream>
#include <string.h>

void Counter::writeCountArray(const std::string& filename, uint64_t *countArray) {
    writeCountArray(filename, countArray, 1024);
}

void Counter::writeCountArray(const std::string& filename, uint64_t *countArray, size_t bufferSize) {
    memset(countArray, 0, sizeof(uint64_t) * 256);

    std::ifstream inputFile(filename, std::ios::in | std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    char *buffer = (char *) malloc(sizeof(char) * bufferSize);
    if (buffer == nullptr) {
        std::cerr << "Failed to allocate buffer" << std::endl;
        inputFile.close();
        return;
    }

    while (inputFile.read(buffer, bufferSize) || inputFile.gcount() > 0) {
        std::streamsize bytesRead = inputFile.gcount();
        for (std::streamsize i = 0; i < bytesRead; ++i) {
            countArray[(unsigned char)buffer[i]] ++;
        }
    }

    free(buffer);
    inputFile.close();
}

/*
std::unordered_map<char, int> Counter::getCountUnorderedMap(const std::string& filename) {
	char character;
	std::ifstream inputFile(filename, std::ios::in);
	std::unordered_map<char, int> countMap;

	const size_t bufferSize = 1024;
    char *buffer = (char *) malloc(sizeof(char) * bufferSize);

    while (inputFile) {
        inputFile.read(buffer, bufferSize);
        std::streamsize bytesRead = inputFile.gcount();
        for (std::streamsize i = 0; i < bytesRead; ++i) {
            ++countMap[buffer[i]];
        }
    }

	free(buffer);
	
	inputFile.close();

	return countMap;
}

std::unordered_map<char, int> Counter::getCountUnorderedMap(const std::string& filename, size_t bufferSize) {
	char character;
	std::ifstream inputFile(filename, std::ios::in);
	std::unordered_map<char, int> countMap;

    char *buffer = (char *) malloc(sizeof(char) * bufferSize);

    while (inputFile) {
        inputFile.read(buffer, bufferSize);
        std::streamsize bytesRead = inputFile.gcount();
        for (std::streamsize i = 0; i < bytesRead; ++i) {
            ++countMap[buffer[i]];
        }
    }

	free(buffer);
	
	inputFile.close();

	return countMap;
}
*/