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
    Bits(uint64_t *bt,unsigned int n);
    ~Bits();
    const Bits &operator=(const Bits &bt);
    operator char*() const;
    unsigned int to_raw(uint64_t **dest) const;
    unsigned int size() const;
    void set(unsigned int pos,bool value);
    void flip(unsigned int pos);
    bool get(unsigned int pos) const;
};
