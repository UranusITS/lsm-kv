#include"sstable.h"
SSTable::SSTable(time_t time_stamp,uint64_t sz,uint64_t min_key,uint64_t max_key,const BloomFilter &bf):
    time_stamp(time_stamp),sz(sz),min_key(min_key),max_key(max_key),bf(bf) {}
SSTable::SSTable(const MemTable &memtable)
{
    time_stamp=time(nullptr);
    sz=memtable.num();
    min_key=memtable.min();
    max_key=memtable.max();
    memtable.scan(min_key,max_key,list);
    bf=BloomFilter(list);
}
void SSTable::read(std::ifstream &in)
{
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
void SSTable::write(std::ofstream &out) const
{
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
