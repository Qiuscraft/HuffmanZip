#include "BitInBuffer.h"

BitInBuffer::BitInBuffer(std::ifstream &inStream): 
    inStream(inStream), data(std::bitset<8>()), current(8) {}

bool BitInBuffer::readBit() {
    if (current == 8) {
        char c;
        inStream.get(c);
        data = std::bitset<8>(c);
        current = 0;
    }

    bool bit = data.test(current);
    current ++;
    return bit;
}
