#ifndef GUARD_HuffmanCode_h
#define GUARD_HuffmanCode_h

#include <bitset>
#include <iostream>

#define HUFFMAN_CODE_MAX_LENGTH 255

class HuffmanCode {
private:
    std::bitset<HUFFMAN_CODE_MAX_LENGTH> data;
    size_t size;
public:
    HuffmanCode();

    HuffmanCode append(bool bit) const;

    size_t getSize() const;
    bool get(size_t index) const;

    friend std::ostream& operator<<(std::ostream& os, const HuffmanCode& code);
};

#endif
