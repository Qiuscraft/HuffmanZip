#ifndef GUARD_DynamicBitSet_h
#define GUARD_DynamicBitSet_h

#include <cstdint>

class DynamicBitSet {
private:
    uint64_t capacity;
    uint64_t size;
    uint64_t front;
    uint64_t data_size;
    uint64_t *data;

public:
    DynamicBitSet(uint64_t capacity);
    DynamicBitSet(const DynamicBitSet &other);
    ~DynamicBitSet();
    DynamicBitSet & operator=(const DynamicBitSet &other);

    void in(bool bit);
    bool out();

    bool empty() const;
    bool full() const;
};

#endif

