#include "Counter.h"
#include "HuffmanTree.h"
#include "HuffmanZip.h"
#include <iostream>
#include <string>
#include <chrono>

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    start = std::chrono::high_resolution_clock::now();
    HuffmanZip::compress("input", "huffmanzip");
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Compression took " << duration.count() << " seconds." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    HuffmanZip::decompress("huffmanzip", "decompressed");
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Decompression took " << duration.count() << " seconds." << std::endl;

    return 0;
}
