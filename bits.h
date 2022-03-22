#pragma once
#include<cstdint>
class Bits
{
private:
    uint64_t *bits;
public:
    Bits(unsigned int n);
    ~Bits();
    void set(unsigned int pos, bool value);
    void flip(unsigned int pos);
    bool get(unsigned int pos) const;
};
