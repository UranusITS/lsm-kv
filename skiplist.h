#pragma once
#include<cstdint>
#include<string>
#include<random>
#include<list>
const unsigned int MAX_LEVEL=8;
class SkipList
{
private:
    struct Node
    {
        uint64_t key;
        std::string s;
        Node *forwards[MAX_LEVEL];
        Node(uint64_t key,const std::string &s);
    }*head,*tail;
    std::default_random_engine random_engine;
    std::uniform_int_distribution<unsigned int> *random_int;
    unsigned int random_level();
public:
    SkipList();
    ~SkipList();
    void put(uint64_t key,const std::string &s);
    std::string get(uint64_t key) const;
    bool del(uint64_t key);
    void reset();
    void scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const;
    void to_sstable() const;
};
