#include"bloomfilter.h"
#include"MurmurHash3.h"
int BloomFilter::MAX_SZ=10240;
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
BloomFilter::~BloomFilter()
{
    delete bits;
}
const BloomFilter &BloomFilter::operator=(const BloomFilter &bf)
{
    bits=bf.bits;
    return *this;
}
BloomFilter::operator char*() const
{
    return (char*)bits;
}
void BloomFilter::insert(uint64_t elem)
{
    unsigned int hash[4];
    MurmurHash3_x64_128(&elem,sizeof(elem),1,hash);
    for(unsigned int i=0;i<4;++i)
        bits->set(hash[i]%MAX_SZ,1);
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
