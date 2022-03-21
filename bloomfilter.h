#pragma once
#include<cstdint>
#include<list>
#include<string>
class BloomFilter
{
private:

public:
    BloomFilter();
    BloomFilter(const std::list<std::pair<uint64_t,std::string>> &list);
    ~BloomFilter();
    operator char*() const;
};
