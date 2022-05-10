#include"sstable.h"
#include<fstream>
SSInfo::SSInfo(const SSTable &sstable,const std::string &filename):
    time_stamp(sstable.time_stamp),min_key(sstable.min_key),max_key(sstable.max_key),filename(filename) {}
SSInfo::SSInfo(time_t time_stamp,uint64_t min_key,uint64_t max_key,const std::string &filename):
    time_stamp(time_stamp),min_key(min_key),max_key(max_key),filename(filename) {}
SSTable::SSTable(const MemTable &memtable):
    time_stamp(time(nullptr)),sz(memtable.num()),min_key(memtable.min()),max_key(memtable.max()),bf(BloomFilter(list))
{
    memtable.scan(min_key,max_key,list);
}
SSTable::SSTable(const std::list<std::pair<uint64_t,std::string>> &new_list):
    time_stamp(time(nullptr)),sz(list.size()),bf(BloomFilter(list)),list(new_list)
{
    min_key=UINT64_MAX;
    max_key=0;
    for(auto it=list.begin();it!=list.end();it++)
    {
        min_key=std::min(min_key,it->first);
        max_key=std::max(max_key,it->first);
    }
}
SSTable::SSTable(const std::string &filename)
{
    std::ifstream in(filename);
    in.read((char*)&time_stamp,8);
    in.read((char*)&sz,8);
    in.read((char*)&min_key,8);
    in.read((char*)&max_key,8);
    in.read((char*)&bf,10240);
    uint32_t offset=0;
    uint64_t key=0;
    std::string value="";
    for(uint64_t i=0;i<sz;++i)
    {
        in.read((char*)&key,8);
        in.read((char*)&offset,4);
        list.push_back(std::make_pair(key,std::string("")));
    }
    char ch='\0';
    for(auto &it:list)
        while(ch=in.get())
            it.second.push_back(ch);
}
std::list<std::pair<uint64_t,std::string>> SSTable::get_list() const
{
    return list;
}
void SSTable::write(const std::string &filename) const
{
    std::ofstream out(filename);
    out.write((char*)&time_stamp,8);
    out.write((char*)&sz,8);
    out.write((char*)&min_key,8);
    out.write((char*)&max_key,8);
    out.write((char*)&bf,10240);
    uint32_t offset=MemTable::INIT_SZ+12*sz;
    for(auto &it:list)
    {
        out.write((char*)&it.first,8);
        out.write((char*)&offset,4);
        offset+=it.second.length()+1;
    }
    for(auto &it:list)
    {
        out.write(it.second.c_str(),it.second.length());
        out.write((char*)'\0',1);
    }
}
void SSTable::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const
{
    for(auto &x: this->list)
        if(x.first>=key1&&x.first<=key2)
            list.push_back(x);
}
