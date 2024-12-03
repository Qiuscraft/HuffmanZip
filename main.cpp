#include "Counter.h"
#include "HuffmanTree.h"
#include "HuffmanZip.h"
#include <iostream>
#include <string>
#include <chrono>

int main(int argc, char *argv[]) {
    // 计时用
    auto start = std::chrono::high_resolution_clock::now();

    std::string mode(argv[1]);
    std::string input(argv[2]);
    std::string output(argv[3]);
    std::string password;
    bool overwrite = false;

    for (int i = 4; i < argc; ++i) {
        if (std::string(argv[i]) == "-p" && i + 1 < argc) {
            password = argv[++i];
        } else if (std::string(argv[i]) == "-y") {
            overwrite = true;
        }
    }

    if (mode == "-c" || mode == "-compress") {
        HuffmanZip::compress(input, output, password);
    } else if (mode == "-d" || mode == "-decompress") {
        if (overwrite) HuffmanZip::decompress(input, output, password);
        else HuffmanZip::decompress(input, output, password, std::cin, std::cout);
    } else {
        std::cerr << "Usage:\n";
        std::cerr << "  huffmanzip -c(ompress) <input> <output> [-p <password>]\n";
        std::cerr << "  huffmanzip -d(ecompress) <input> <output> [-p <password>] [-y]\n";
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "It took " << duration.count() << " seconds." << std::endl;

    return 0;
}
