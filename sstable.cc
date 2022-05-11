#include"sstable.h"
#include<fstream>
#include<iostream>
SSInfo::SSInfo(const std::string &fullname,const std::string &filename):
    filename(filename)
{
    uint64_t sz;
    std::ifstream in(fullname,std::ios::binary);
    in.read((char*)&time_stamp,8);
    in.read((char*)&sz,8);
    in.read((char*)&min_key,8);
    in.read((char*)&max_key,8);
    SSTable::time_stamp_cnt=std::max(SSTable::time_stamp_cnt,time_stamp+1);
    in.close();
}
SSInfo::SSInfo(const SSTable &sstable,const std::string &filename):
    time_stamp(sstable.time_stamp),min_key(sstable.min_key),max_key(sstable.max_key),filename(filename) {}
SSInfo::SSInfo(uint64_t time_stamp,uint64_t min_key,uint64_t max_key,const std::string &filename):
    time_stamp(time_stamp),min_key(min_key),max_key(max_key),filename(filename) {}
bool SSInfo::operator<(const SSInfo &ssinfo) const
{
    return time_stamp>ssinfo.time_stamp;
}
uint64_t SSTable::time_stamp_cnt=0;
SSTable::SSTable(const MemTable &memtable):
    time_stamp(time_stamp_cnt++),sz(memtable.get_num()),min_key(memtable.get_min()),max_key(memtable.get_max())
{
    list=memtable.get_list();
    bf=BloomFilter(list);
}
SSTable::SSTable(const MemTable *memtable):
    time_stamp(time_stamp_cnt++),sz(memtable->get_num()),min_key(memtable->get_min()),max_key(memtable->get_max())
{
    list=memtable->get_list();
    bf=BloomFilter(list);
}
SSTable::SSTable(const std::list<std::pair<uint64_t,std::string>> &new_list):
    time_stamp(time_stamp_cnt++),sz(new_list.size())
{
    list=new_list;
    bf=BloomFilter(new_list);
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
    //std::cout<<"Reading: "<<filename<<std::endl;
    char ignore_buf[BloomFilter::MAX_SZ];
    bf=BloomFilter(filename);
    std::ifstream in(filename,std::ios::binary);
    in.read((char*)&time_stamp,8);
    in.read((char*)&sz,8);
    in.read((char*)&min_key,8);
    in.read((char*)&max_key,8);
    in.read((char*)&ignore_buf,BloomFilter::MAX_SZ);
    uint32_t offset=0;
    uint64_t key=0;
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
    in.close();
}
std::list<std::pair<uint64_t,std::string>> SSTable::get_list() const
{
    return list;
}
void SSTable::write(const std::string &filename) const
{
    /**
    std::cout<<"Writing: "<<filename<<std::endl;
    std::cout<<"time_stamp: "<<time_stamp<<std::endl;
    std::cout<<"size: "<<sz<<std::endl;
    std::cout<<"min_key: "<<min_key<<std::endl;
    std::cout<<"max_key: "<<max_key<<std::endl;
    */
    std::ofstream out(filename,std::ios::binary);
    out.write((char*)&time_stamp,8);
    out.write((char*)&sz,8);
    out.write((char*)&min_key,8);
    out.write((char*)&max_key,8);
    uint64_t *bf_data;
    unsigned int bf_sz=bf.to_raw(&bf_data);
    for(unsigned int i=0;i<bf_sz;i++)
        out.write((char*)&bf_data[i],8);
    delete[] bf_data;
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
        out.write("\0",1);
    }
    out.close();
}
std::string SSTable::get(uint64_t key) const
{
    for(auto &x:list)
        if(x.first==key)
            return x.second;
    return "";
}
void SSTable::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const
{
    for(auto &x:this->list)
        if(x.first>=key1&&x.first<=key2)
        {
            auto it=list.begin();
            while(it!=list.end()&&it->first<x.first)
                it++;
            if(it==list.end()||it->first>x.first)
                list.insert(it,x);
        }
}
