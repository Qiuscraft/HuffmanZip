#ifndef GUARD_BitOutBuffer_h
#define GUARD_BitOutBuffer_h

#include <fstream>
#include "HuffmanCode.h"

#define DATA_ARRAY_SIZE 4
#define DATA_CAPACITY (DATA_ARRAY_SIZE * 128)
#define DATA_BIT_PER_ELEMENT 128

class BitOutBuffer {
private:
    std::ofstream &outStream;
    __uint128_t data[DATA_ARRAY_SIZE]; // 使用DATA_SIZE个128位的数据，构造容器。
    size_t size;

    void in(bool bit, int n);
    void leftShift(size_t shiftSize); // 末尾不补零
    void set(size_t index, bool bit);
    bool get(size_t index) const;
    char getByte(size_t byteIndex) const;

public:
    BitOutBuffer(std::ofstream &fileStream);

    void write(const HuffmanCode &code);
    int finalWriteWithPaddings();
};

#endif
