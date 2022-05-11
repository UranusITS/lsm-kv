#pragma once
#include"bloomfilter.h"
#include"memtable.h"
#include<ctime>
class SSTable;
struct SSInfo
{
    uint64_t time_stamp,min_key,max_key;
    std::string filename;
    SSInfo(const std::string &fullname,const std::string &filename);
    SSInfo(const SSTable &sstable,const std::string &filename);
    SSInfo(uint64_t time_stamp,uint64_t min_key,uint64_t max_key,const std::string &filename);
    bool operator<(const SSInfo &ssinfo) const;
};
class SSTable
{
friend SSInfo;
private:
    static uint64_t time_stamp_cnt;
    uint64_t time_stamp,sz,min_key,max_key;
    BloomFilter bf;
    std::list<std::pair<uint64_t,std::string>>list;
public:
    SSTable(const MemTable &memtable);
    SSTable(const MemTable *memtable);
    SSTable(const std::string &filename);
    SSTable(const std::list<std::pair<uint64_t,std::string>> &new_list);
    std::list<std::pair<uint64_t,std::string>> get_list() const;
    void write(const std::string &filename) const;
    std::string get(uint64_t key) const;
    void scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const;
};
