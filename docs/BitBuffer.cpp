#include "BitBuffer.h"
#include <algorithm>
#include <stdexcept>

BitOutBuffer::BitOutBuffer(size_t size): data(std::vector<bool>(size)) {}

bool BitOutBuffer::empty() const {
    return data.size() == 0;
}

bool BitOutBuffer::full() const {
    return data.size() == data.capacity();
}

void BitOutBuffer::in(bool bit) {
    if (full())
        throw std::domain_error("Cannot input a bit to a BitOutBuffer. (Full)");
    data[(front + size + 1) % maxSize] = bit;
    size += 1;
}

void BitOutBuffer::in(bool *bitData, uint64_t n) {
    if (maxSize - size < n)
        throw std::domain_error("Cannot input data to a BitOutBuffer. (Not Enough Space)");
    for (uint64_t i = 0; i < n; i++) {
        data[(front + size + i) % maxSize] = bitData[i];
        size += 1;
    }
}

bool BitOutBuffer::out() {
    if (empty())
        throw std::runtime_error("Failed to out bit from empty buffer.");
    size --;
    bool ret = data[front];
    front = (front + 1) % maxSize;
    return ret;
}

// 将比特流写入文件的函数，不处理剩余不足8位的情况
uint64_t BitOutBuffer::writeToFile(std::ofstream &outFile) {
    // 前置检查
    if (!outFile.is_open()) {
        throw std::invalid_argument("Output file is not open.");
    }

    if (empty()) {
        return 0;
    }

    // 计算可以写入的完整字节数（每8位为1字节）
    uint64_t bytes_to_write = size / 8;
    if (bytes_to_write == 0) {
        return 0;
    }

    // 写入字节
    unsigned char byte;
    uint64_t current = front;
    for(uint64_t byte_num = 0; byte_num < bytes_to_write; ++byte_num) {
        byte = 0;
        for(int bit = 0; bit < 8; ++bit) {
            byte = (byte << 1) | static_cast<unsigned char>(data[current]);
            current = (current + 1) % maxSize;
        }
        outFile.put(byte);
        if(!outFile) {
            throw std::runtime_error("Error writing to file."); // 如果写入失败，抛出异常
        }
    }

    // 更新缓冲区的前端位置和大小
    front = (front + bytes_to_write * 8) % maxSize;
    size -= bytes_to_write * 8;

    return bytes_to_write; // 返回实际写入的字节数
}

uint64_t BitOutBuffer::writeToFileWithZero(std::ofstream &outFile, uint32_t &padding_size) {
    uint64_t bytes_ret = writeToFile(outFile);
    unsigned char c = 0;
    uint64_t current = front;

    for(int bit = 0; bit < size; ++bit){
        c = (c << 1) | data[current];
        current = (current + 1) % maxSize;
    }
    c <<= (8 - size);
    padding_size = 8 - size;

    outFile.put(c);
    if(!outFile){
        throw std::runtime_error("Error writing to file.");
    }

    front = (front + size) % maxSize;
    size = 0;

    return bytes_ret + 1;
}
