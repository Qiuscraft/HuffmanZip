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

    std::string output;
    if (argc > 3) output = argv[3];
    else {
        if (mode == "-c" || mode == "-compress") output = input + ".huffmanzip";
        else output = ".";
    }

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
        try {
            HuffmanZip::compress(input, output, password, std::cin, std::cout, overwrite);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    } else if (mode == "-d" || mode == "-decompress") {
        try {
            HuffmanZip::decompress(input, output, password, std::cin, std::cout, overwrite);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Usage:\n";
        std::cerr << "  huffmanzip -c(ompress) <input> [<output>] [-p <password>] [-y]\n";
        std::cerr << "  huffmanzip -d(ecompress) <input> [<output>] [-p <password>] [-y]\n";
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "It took " << duration.count() << " seconds." << std::endl;

    return 0;
}
