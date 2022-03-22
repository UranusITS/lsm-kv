#include"bloomfilter.h"
#include"MurmurHash3.h"
int BloomFilter::MAX_SZ=2560;
BloomFilter::BloomFilter()
{
    for(unsigned int i=0;i<4;++i)
        bits[i]=new Bits(MAX_SZ);
}
BloomFilter::BloomFilter(const std::list<std::pair<uint64_t,std::string>> &list)
{
    for(unsigned int i=0;i<4;++i)
        bits[i]=new Bits(MAX_SZ);
    for(auto &it:list)
        insert(it.first);
}
BloomFilter::~BloomFilter()
{
    for(unsigned int i=0;i<4;++i)
        delete bits[i];
    delete[] bits;
}
BloomFilter::operator char*() const
{
    return (char *)bits;
}
void BloomFilter::insert(uint64_t elem)
{
    unsigned int hash[4];
    MurmurHash3_x64_128(&elem,sizeof(elem),1,hash);
    for(unsigned int i=0;i<4;++i)
        bits[i]->set(hash[i]%MAX_SZ,1);
}
bool BloomFilter::find(uint64_t elem) const
{
    unsigned int hash[4];
    MurmurHash3_x64_128(&elem,sizeof(elem),1,hash);
    for(unsigned int i=0;i<4;++i)
        if(bits[i]->get(hash[i]%MAX_SZ)==false)
            return false;
    return true;
}
