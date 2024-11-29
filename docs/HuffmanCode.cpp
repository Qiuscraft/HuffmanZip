#include "HuffmanCode.h"
#include <algorithm>

// vector当元素个数达到容量时，会进行扩容，将vector大小设为size + 1防止扩容。
HuffmanCode::HuffmanCode(size_t size): data(std::vector<bool>(size + 1)) {}

HuffmanCode HuffmanCode::append(bool bit) {
    // 手动生成vector，防止vector扩容导致内存碎片化
    HuffmanCode code = HuffmanCode(data.size() + 1);
    std::copy(data.begin(), data.end(), code.data.begin());
    code.data.push_back(bit);
    return code;
}

size_t HuffmanCode::getSize() const {
    return data.size();
}
