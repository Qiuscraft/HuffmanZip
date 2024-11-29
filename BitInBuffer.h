#ifndef GUARD_BitInBuffer_h
#define GUARD_BitInBuffer_h

#include <bitset>
#include <fstream>
#include "HuffmanCode.h"

class BitInBuffer {
private:
    std::ifstream &inStream;
    std::bitset<8> data;
    size_t current;

public:
    BitInBuffer(std::ifstream &inStream);

    bool readBit();
};

#endif
