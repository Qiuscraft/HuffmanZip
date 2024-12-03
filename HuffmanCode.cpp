// 哈夫曼编码模块，使用std::bitset存储编码，比使用std::string更高效。

#include "HuffmanCode.h"

HuffmanCode::HuffmanCode(): data(std::bitset<255>()), size(0){}

HuffmanCode HuffmanCode::append(bool bit) const {
    HuffmanCode code = HuffmanCode(*this);
    code.size ++;
    code.data.set(size, bit);
    return code;
}

size_t HuffmanCode::getSize() const {
    return size;
}

bool HuffmanCode::get(size_t index) const {
    return data.test(index);
}

std::ostream& operator<<(std::ostream& os, const HuffmanCode& code) {
    for (size_t i = 0; i < code.size; i++) {
        os << code.data[i];
    }
    return os;
}