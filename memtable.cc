#include"memtable.h"
#include<ctime>
#include<iostream>
const uint64_t MemTable::INIT_SZ=10272;
const uint64_t MemTable::MAX_SZ=2097152;
MemTable::MemTable()
{
    skip_list=new SkipList;
    sz=INIT_SZ;
    min_key=UINT64_MAX;
    max_key=0;
}
MemTable::~MemTable()
{
    delete skip_list;
}
uint64_t MemTable::get_size() const
{
    return sz;
}
uint64_t MemTable::get_num() const
{
    return skip_list->size();
}
uint64_t MemTable::get_min() const
{
    return min_key;
}
uint64_t MemTable::get_max() const
{
    return max_key;
}
std::list<std::pair<uint64_t,std::string>> MemTable::get_list() const
{
    std::list<std::pair<uint64_t,std::string>>tmp;
    skip_list->scan(min_key,max_key,tmp);
    return tmp;
}
bool MemTable::put(uint64_t key,const std::string &s)
{
    //std::cout<<sz<<' '<<s.length()<<' '<<MAX_SZ<<std::endl;
    if(sz+13+s.length()>MAX_SZ) return false;
    if(key<min_key) min_key=key;
    if(key>max_key) max_key=key;
    skip_list->put(key,s);
    sz+=(13+s.length());
    return true;
}
std::string MemTable::get(uint64_t key)
{
    return skip_list->get(key);
}
bool MemTable::del(uint64_t key)
{
    std::string s=skip_list->get(key);
    if(s=="") return false;
    sz-=(13+s.length());
    return skip_list->del(key);
}
void MemTable::reset()
{
    skip_list->reset();
    sz=INIT_SZ;
    min_key=UINT64_MAX;
    max_key=0;
}
void MemTable::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const
{
    return skip_list->scan(key1,key2,list);
}
