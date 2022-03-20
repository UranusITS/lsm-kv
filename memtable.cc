#include"memtable.h"
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
uint64_t MemTable::size() const
{
    return sz;
}
void MemTable::put(uint64_t key,const std::string &s)
{
    if(sz+12+s.length()>MAX_SZ)
    {
        to_sstable();
        reset();
    }
    if(key<min_key) min_key=key;
    if(key>max_key) max_key=key;
    skip_list->put(key,s);
    sz+=12+s.length();
}
std::string MemTable::get(uint64_t key)
{
    return skip_list->get(key);
}
bool MemTable::del(uint64_t key)
{
    return skip_list->del(key);
}
void MemTable::reset()
{
    skip_list->reset();
    sz=INIT_SZ;
    min_key=UINT64_MAX;
    max_key=0;
}
void MemTable::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list)
{
    return skip_list->scan(key1,key2,list);
}
void MemTable::to_sstable() const
{
    //TODO
}
