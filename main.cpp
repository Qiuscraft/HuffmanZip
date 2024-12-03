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

    if (argc < 3) {
        cerr << "Usage:\n";
        cerr << "  huffmanzip -c(ompress) <input> [<output>] [-p <password>] [-y]\n";
        cerr << "  huffmanzip -d(ecompress) <input> [<output>] [-p <password>] [-y]\n";
        return 1;
    }

    string mode = argv[1];
    string input = argv[2];

    string output;
    string password;
    bool overwrite = false;

    // Initialize to check if optional arguments have been reached
    bool optionalArgsStarted = false;

    // Start parsing from the third argument
    for (int i = 3; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "-p") {
            if (i + 1 < argc) {
                password = argv[++i];
            } else {
                cerr << "Error: Missing password after -p option.\n";
                return 1;
            }
        } else if (arg == "-y") {
            overwrite = true;
        } else if (arg.size() > 0 && arg[0] == '-') {
            cerr << "Unknown option: " << arg << endl;
            return 1;
        } else if (!optionalArgsStarted) {
            // Assume it's the output file if no optional arguments have started
            output = arg;
            optionalArgsStarted = true;
        } else {
            cerr << "Unexpected argument: " << arg << endl;
            return 1;
        }
    }

    // Set a default output filename if none was provided
    if (output.empty()) {
        if (mode == "-c" || mode == "-compress") {
            output = input + ".huffmanzip";
        } else {
            output = ".";
        }
    }

    // Invoke the corresponding compress or decompress function
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
