#pragma once
#include"bits.h"
#include<cstdint>
#include<list>
#include<string>
class BloomFilter
{
private:
    static int MAX_SZ;
    Bits *bits[4];
public:
    BloomFilter();
    BloomFilter(const std::list<std::pair<uint64_t,std::string>> &list);
    ~BloomFilter();
    operator char*() const;
    void insert(uint64_t elem);
    bool find(uint64_t elem) const;
};
