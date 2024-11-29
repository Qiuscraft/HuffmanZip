#ifndef GUARD_BitOutBuffer_h
#define GUARD_BitOutBuffer_h

#include <bitset>
#include <fstream>
#include "HuffmanCode.h"

#define SET_SIZE 512

class BitOutBuffer {
private:
    std::ofstream &outStream;
    std::bitset<SET_SIZE> data;
    size_t size;

    void in(bool bit, int n);

public:
    BitOutBuffer(std::ofstream &fileStream);

    void write(const HuffmanCode &code);
    int finalWriteWithPaddings();
};

#endif
