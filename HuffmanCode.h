// 哈夫曼编码模块，使用std::bitset存储编码，比使用std::string更高效。

#ifndef GUARD_HuffmanCode_h
#define GUARD_HuffmanCode_h

#include <bitset>
#include <iostream>

#define HUFFMAN_CODE_MAX_LENGTH 255

class HuffmanCode {
private:
    std::bitset<255> data;
    size_t size;
public:
    HuffmanCode();

    HuffmanCode append(bool bit) const;

    size_t getSize() const;
    bool get(size_t index) const;

    friend std::ostream& operator<<(std::ostream& os, const HuffmanCode& code);
};

#endif
