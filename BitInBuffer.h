// 解压缩过程中用到的缓冲区模块，从文件中读取一个字节，并提供按位输出的函数。
// 按位输出使得程序能够在哈夫曼树上方便地寻找有效数据结点。

#ifndef GUARD_BitInBuffer_h
#define GUARD_BitInBuffer_h

#include <bitset>
#include <fstream>
#include "HuffmanCode.h"

class BitInBuffer {
private:
    std::ifstream &inStream;
    std::bitset<8> data;
    size_t current;

public:
    BitInBuffer(std::ifstream &inStream);

    bool readBit();
};

#endif
