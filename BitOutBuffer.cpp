#include "BitOutBuffer.h"
#include "HuffmanCode.h"

#include "Util.h"

// TODO: 处理小端序和大端序的问题。

BitOutBuffer::BitOutBuffer(std::ofstream &outStream):
    outStream(outStream), size(0) {}

void BitOutBuffer::write(const HuffmanCode &code) {
    for (int i = 0; i < code.getSize(); i++) {
        set(size, code.get(i));
        size ++;
    }
    if (size > DATA_CAPACITY - HUFFMAN_CODE_MAX_LENGTH) {
        const int bytes = size / 8;
        for (int i = 0; i < bytes; i++) {
            outStream.put(getByte(i));
        }
        leftShift(bytes * 8);
        size -= bytes * 8;
    }
}

int BitOutBuffer::finalWriteWithPaddings() {
    int paddings = size % 8 == 0 ? 0: 8 - (size % 8);
    in(0, paddings);

    for (int i = 0; i < size / 8; i++) {
        outStream.put(getByte(i));
    }

    size = 0;

    return paddings;
}

void BitOutBuffer::in(bool bit, int n) {
    if (DATA_CAPACITY - size < n)
        throw std::domain_error("Not Enough Space for BitOutBuffer::in.");
    
    for (int i = 0; i < n; i++) {
        set(size, bit);
        size ++;
    }
}

void BitOutBuffer::leftShift(size_t shiftSize) {
    if (shiftSize == 0 || shiftSize >= DATA_CAPACITY) return;

    size_t fullElementShift = shiftSize / DATA_BIT_PER_ELEMENT;
    size_t remainingShift = shiftSize % DATA_BIT_PER_ELEMENT;

    // 先处理整数个元素的移位
    if (fullElementShift > 0) {
        for (int i = 0; i < DATA_ARRAY_SIZE - fullElementShift; ++i) {
            data[i] = data[i + fullElementShift];
        }
    }

    // 处理剩余的位移
    if (remainingShift > 0) {
        for (int i = 0; i < DATA_ARRAY_SIZE - 1; ++i) {
            data[i] = (data[i] << remainingShift) | (data[i + 1] >> (DATA_BIT_PER_ELEMENT - remainingShift));
        }
        data[DATA_ARRAY_SIZE - 1] <<= remainingShift;
    }
}


void BitOutBuffer::set(size_t index, bool bit) {
    size_t dataIndex = index / DATA_BIT_PER_ELEMENT;
    size_t bitIndex = DATA_BIT_PER_ELEMENT - 1 - index % DATA_BIT_PER_ELEMENT;
    if (bit) {
        __uint128_t tmp = (__uint128_t)1 << bitIndex;
        data[dataIndex] |= tmp;
    } else {
        __uint128_t tmp = ~((__uint128_t)1 << bitIndex);
        data[dataIndex] &= tmp;
    }
}

bool BitOutBuffer::get(size_t index) const {
    size_t dataIndex = index / DATA_BIT_PER_ELEMENT;
    size_t bitIndex = DATA_BIT_PER_ELEMENT - 1 - index % DATA_BIT_PER_ELEMENT;

    return (bool)((data[dataIndex] >> bitIndex) & 1);
}

char BitOutBuffer::getByte(size_t byteIndex) const {
    size_t index = byteIndex * 8;
    size_t dataIndex = index / DATA_BIT_PER_ELEMENT;
    size_t bitIndex = DATA_BIT_PER_ELEMENT - 1 - index % DATA_BIT_PER_ELEMENT;
    return (char)((data[dataIndex] >> (bitIndex - 7)) & 0xff);
}
