#include"bloomfilter.h"
#include"MurmurHash3.h"
#include<fstream>
const int BloomFilter::MAX_SZ=10240;
BloomFilter::BloomFilter()
{
    bits=new Bits(MAX_SZ);
}
BloomFilter::BloomFilter(const std::list<std::pair<uint64_t,std::string>> &list)
{
    bits=new Bits(MAX_SZ);
    for(auto &it:list)
        insert(it.first);
}
BloomFilter::BloomFilter(const BloomFilter &bf)
{
    bits=new Bits(*bf.bits);
}
BloomFilter::BloomFilter(const std::string &filename)
{
    std::ifstream in(filename,std::ios::binary);
    static char ignore_buf[32];
    in.read((char*)ignore_buf,32);
    unsigned int sz=(MAX_SZ-1)/sizeof(uint64_t)+1;
    uint64_t *tmp=new uint64_t[sz];
    for(unsigned int i=0;i<sz;++i)
        in.read((char*)&tmp[i],8);
    bits=new Bits(tmp,sz);
    delete[] tmp;
    in.close();
}
BloomFilter::~BloomFilter()
{
    delete bits;
}
const BloomFilter &BloomFilter::operator=(const BloomFilter &bf)
{
    *bits=*bf.bits;
    return *this;
}
BloomFilter::operator char*() const
{
    return (char*)bits;
}
unsigned int BloomFilter::to_raw(uint64_t **dest) const
{
    return bits->to_raw(dest);
}
void BloomFilter::insert(uint64_t elem)
{
    unsigned int hash[4];
    MurmurHash3_x64_128(&elem,sizeof(elem),1,hash);
    for(unsigned int i=0;i<4;++i)
        bits->set(hash[i]%MAX_SZ,true);
}
bool BloomFilter::find(uint64_t elem) const
{
    unsigned int hash[4];
    MurmurHash3_x64_128(&elem,sizeof(elem),1,hash);
    for(unsigned int i=0;i<4;++i)
        if(!bits->get(hash[i]%MAX_SZ))
            return false;
    return true;
}
