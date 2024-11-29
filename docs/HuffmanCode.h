#ifndef GUARD_HuffmanCode_h
#define GUARD_HuffmanCode_h

#include <vector>

class HuffmanCode {
private:
    std::vector<bool> data;

public:
    HuffmanCode(size_t size);
    
    HuffmanCode append(bool bit);
    
    size_t getSize() const;
    size_t getMaxSize() const;
};

#endif
