#pragma once
#include"bits.h"
#include<cstdint>
#include<list>
#include<string>
class BloomFilter
{
private:
    Bits *bits;
public:
    BloomFilter();
    BloomFilter(const std::list<std::pair<uint64_t,std::string>> &list);
    BloomFilter(const BloomFilter &bf);
    BloomFilter(const std::string &filename);
    ~BloomFilter();
    const static int MAX_SZ;
    const BloomFilter &operator=(const BloomFilter &bf);
    operator char*() const;
    unsigned int to_raw(uint64_t **dest) const;
    void insert(uint64_t elem);
    bool find(uint64_t elem) const;
};
