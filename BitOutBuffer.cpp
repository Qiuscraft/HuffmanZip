#include "BitOutBuffer.h"

BitOutBuffer::BitOutBuffer(std::ofstream &outStream):
    outStream(outStream), data(std::bitset<SET_SIZE>()), size(0) {}

void BitOutBuffer::write(const HuffmanCode &code) {
    for (int i = 0; i < code.getSize(); i++) {
        data.set(size, code.get(i));
        size ++;
    }
    if (size > SET_SIZE - 255) {
        const int bytes = size / 8;
        for (int i = 0; i < bytes; i++) {
            std::bitset<8> bit8;
            for (int j = 0; j < 8; j++) 
                bit8.set(j, data.test(i * 8 + j));
            char c = char(bit8.to_ulong());
            outStream << c;
        }
        data << (bytes * 8);
        size -= bytes * 8;
    }
}

int BitOutBuffer::finalWriteWithPaddings() {
    int paddings = size % 8 == 0 ? 0: 8 - (size % 8);
    in(0, paddings);

    for (int i = 0; i < size / 8; i++) {
        std::bitset<8> bit8;
        for (int j = 0; j < 8; j++) 
            bit8.set(j, data.test(i * 8 + j));
        char c = char(bit8.to_ulong());
        outStream << c;
    }

    data << size;
    size -= size;

    return paddings;
}

void BitOutBuffer::in(bool bit, int n) {
    if (SET_SIZE - size < n)
        throw std::domain_error("Not Enough Space for BitOutBuffer::in.");
    
    for (int i = 0; i < n; i++) {
        data.set(size, bit);
        size ++;
    }
}
