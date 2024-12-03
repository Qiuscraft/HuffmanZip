#include "Counter.h"
#include "HuffmanTree.h"
#include "HuffmanZip.h"
#include <iostream>
#include <string>
#include <chrono>

using namespace std;

int main(int argc, char *argv[]) {
    // 计时用
    auto start = chrono::high_resolution_clock::now();

    string mode(argv[1]);
    string input(argv[2]);

    string output;
    if (argc > 3) output = argv[3];
    else {
        if (mode == "-c" || mode == "-compress") output = input + ".huffmanzip";
        else output = ".";
    }

    string password;
    bool overwrite = false;

    for (int i = 4; i < argc; ++i) {
        if (string(argv[i]) == "-p" && i + 1 < argc) {
            password = argv[++i];
        } else if (string(argv[i]) == "-y") {
            overwrite = true;
        }
    }

    if (mode == "-c" || mode == "-compress") {
        try {
            HuffmanZip::compress(input, output, password, cin, cout, overwrite);
        } catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
    } else if (mode == "-d" || mode == "-decompress") {
        try {
            HuffmanZip::decompress(input, output, password, cin, cout, overwrite);
        } catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
    } else {
        cerr << "Usage:\n";
        cerr << "  huffmanzip -c(ompress) <input> [<output>] [-p <password>] [-y]\n";
        cerr << "  huffmanzip -d(ecompress) <input> [<output>] [-p <password>] [-y]\n";
        return 1;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "It took " << duration.count() << " seconds." << endl;

    return 0;
}
