#pragma once
#include<cstdint>
class Bits
{
private:
    unsigned int sz;
    uint64_t *bits;
public:
    Bits(unsigned int n);
    Bits(const Bits &bt);
    ~Bits();
    const Bits &operator=(const Bits &bt);
    operator char*() const;
    unsigned int size() const;
    void set(unsigned int pos, bool value);
    void flip(unsigned int pos);
    bool get(unsigned int pos) const;
};
