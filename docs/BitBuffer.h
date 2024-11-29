#ifndef GUARD_BitBuffer_h
#define GUARD_BitBuffer_h

#include <fstream>
#include <vector>

class BitOutBuffer {
private:
    std::vector<bool> data;
    

public:
    BitOutBuffer(size_t size);

    bool empty() const;
    bool full() const;

    void in(bool *bitData, uint64_t n);
    void in(bool bit);
    bool out();

    uint64_t writeToFile(std::ofstream &outFile);
    uint64_t writeToFileWithZero(std::ofstream &outFile, uint32_t &padding_size);
};

#endif
