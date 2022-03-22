#include"bits.h"
#include<cstring>
Bits::Bits(unsigned int n)
{
    bits=new uint64_t[(n-1)/sizeof(uint64_t)+1];
    for(unsigned int i=0;i<=(n-1)/sizeof(uint64_t);++i)
        bits[i]=0;
}
Bits::~Bits()
{
    delete[] bits;
}
void Bits::set(unsigned int pos, bool value)
{
    bits[pos/sizeof(uint64_t)]&=(value<<(pos%sizeof(uint64_t)));
}
void Bits::flip(unsigned int pos)
{
    bits[pos/sizeof(uint64_t)]^=(1<<(pos%sizeof(uint64_t)));
}
bool Bits::get(unsigned int pos) const
{
    return (bits[pos/sizeof(uint64_t)]>>(pos%sizeof(uint64_t)))&1;
}
