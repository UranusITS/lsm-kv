#include"bits.h"
#include<cstring>
Bits::Bits(unsigned int n)
{
    sz=(n-1)/sizeof(uint64_t)+1;
    bits=new uint64_t[sz];
    for(unsigned int i=0;i<sz;++i)
        bits[i]=0;
}
Bits::Bits(const Bits &bt)
{
    sz=bt.size();
    bits=new uint64_t[sz];
    for(unsigned int i=0;i<sz;++i)
        bits[i]=bt.bits[i];
}
Bits::~Bits()
{
    delete[] bits;
}
const Bits &Bits::operator=(const Bits &bt)
{
    delete bits;
    sz=bt.size();
    bits=new uint64_t[sz];
    for(unsigned int i=0;i<sz;++i)
        bits[i]=bt.bits[i];
    return *this;
}
Bits::operator char*() const
{
    return (char*)bits;
}
unsigned int Bits::size() const
{
    return sz;
}
void Bits::set(unsigned int pos, bool value)
{
    if(value) bits[pos/sizeof(uint64_t)]&=(1<<(pos%sizeof(uint64_t)));
    else bits[pos/sizeof(uint64_t)]&=(~(1<<(pos%sizeof(uint64_t))));
}
void Bits::flip(unsigned int pos)
{
    bits[pos/sizeof(uint64_t)]^=(1<<(pos%sizeof(uint64_t)));
}
bool Bits::get(unsigned int pos) const
{
    return (bits[pos/sizeof(uint64_t)]>>(pos%sizeof(uint64_t)))&1;
}
