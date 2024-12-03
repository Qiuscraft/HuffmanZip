#include "HuffmanCode.h"

HuffmanCode::HuffmanCode(): data(std::bitset<HUFFMAN_CODE_MAX_LENGTH>()), size(0){}

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
