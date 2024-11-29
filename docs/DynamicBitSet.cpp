#include "DynamicBitSet.h"
#include <math.h>
#include <algorithm>
#include <stdexcept>

DynamicBitSet::DynamicBitSet(uint64_t capacity): 
    capacity(capacity), size(0), front(0),
    data_size((capacity + 63) / 64), data(new uint64_t[data_size]) {}

DynamicBitSet::DynamicBitSet(const DynamicBitSet &other):
    capacity(other.capacity), size(other.size), front(other.front), 
    data_size(other.data_size), data(new uint64_t[data_size]) {
    std::copy(other.data, other.data + data_size, data);
}

DynamicBitSet::~DynamicBitSet() {
    delete[] data;
}

DynamicBitSet & DynamicBitSet::operator=(const DynamicBitSet &other) {
    if (&other != this) {
        delete[] data;
        capacity = other.capacity;
        size = other.size;
        front = other.front;
        data_size = other.data_size;
        data = new uint64_t[data_size];
        std::copy(other.data, other.data + data_size, data);
    }
    return *this;
}

void DynamicBitSet::in(bool bit) {
    if (full())
        throw std::domain_error("Failed to run DynamicBitSet::in because the DynamicBitSet is full.");
    
}

bool DynamicBitSet::empty() const {
    return size == 0;
}

bool DynamicBitSet::full() const {
    return size == capacity;
}
