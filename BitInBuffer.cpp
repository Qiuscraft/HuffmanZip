// 解压缩过程中用到的缓冲区模块，从文件中读取一个字节，并提供按位输出的函数。
// 按位输出使得程序能够在哈夫曼树上方便地寻找有效数据结点。

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

    bool bit = data.test(7 - current);
    current ++;
    return bit;
}
