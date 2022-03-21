#pragma once
#include"bloomfilter.h"
#include"memtable.h"
#include<ctime>
#include<fstream>
class SSTable
{
private:
    time_t time_stamp;
    uint64_t sz,min_key,max_key;
    BloomFilter bf;
    std::list<std::pair<uint64_t,std::string>> list;
public:
    SSTable(time_t time_stamp,uint64_t sz,uint64_t min_key,uint64_t max_key,const BloomFilter &bf);
    SSTable(const MemTable &memtable);
    void read(std::ifstream &in);
    void write(std::ofstream &out) const;
};
