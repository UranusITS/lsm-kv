#pragma once
#include"kvstore_api.h"
#include"skiplist.h"
class MemTable
{
private:
    uint64_t sz,min_key,max_key;
    SkipList *skip_list;
public:
    static uint64_t MAX_SZ;
    static uint64_t INIT_SZ;
    MemTable();
    ~MemTable();
    uint64_t size() const;
    uint64_t num() const;
    uint64_t min() const;
    uint64_t max() const;
    bool put(uint64_t key,const std::string &s);
    std::string get(uint64_t key);
    bool del(uint64_t key);
    void reset();
    void scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const;
};
