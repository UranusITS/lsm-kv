#pragma once
#include"kvstore_api.h"
#include"skiplist.h"
#include<vector>
const uint64_t MAX_SZ=2097152;
const uint64_t INIT_SZ=10272;
class MemTable
{
private:
    uint64_t sz,min_key,max_key;
    SkipList *skip_list;
public:
    MemTable();
    ~MemTable();
    uint64_t size() const;
    void put(uint64_t key,const std::string &s);
    std::string get(uint64_t key);
    bool del(uint64_t key);
    void reset();
    void scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list);
    void to_sstable() const;
};
