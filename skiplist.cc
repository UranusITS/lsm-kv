#include"skiplist.h"
#include<ctime>
SkipList::Node::Node(uint64_t key,const std::string &s):
    key(key),s(s)
{
    for(unsigned int i=0;i<MAX_LEVEL;++i)
        forwards[i]=nullptr;
}
unsigned int SkipList::random_level()
{
    return (*random_int)(random_engine);
}
SkipList::SkipList()
{
    head=new Node(0,"");
    tail=new Node(0,"");
    for(unsigned int i=0;i<MAX_LEVEL;++i)
        head->forwards[i]=tail->forwards[i]=tail;
    random_engine.seed(time(0));
    random_int=new std::uniform_int_distribution<unsigned int>(0,MAX_LEVEL-1);
}
SkipList::~SkipList()
{
    Node *ncur=head;
    while(ncur!=tail)
    {
        Node *ntmp=ncur;
        ncur=ncur->forwards[0];
        delete ntmp;
    }
    delete ncur;
}
void SkipList::put(uint64_t key,const std::string &s)
{
    Node *ntmp=new Node(key,s);
    for(unsigned int i=0;i<MAX_LEVEL;++i)
        ntmp->forwards[i]=tail;
    Node *ncur=head;
    for(unsigned int i=random_level();i>=0;--i)
    {
        while(ncur->forwards[i]!=tail&&ncur->forwards[i]->key<key)
            ncur=ncur->forwards[i];
        ncur->forwards[i]=ntmp;
    }
}
std::string SkipList::get(uint64_t key) const
{
    Node *ncur=head;
    for(unsigned int i=MAX_LEVEL-1;i>=0;--i)
        while(ncur->forwards[i]!=tail&&ncur->forwards[i]->key<key)
            ncur=ncur->forwards[i];
    if(ncur->forwards[0]!=tail&&ncur->forwards[0]->key==key)
        return ncur->forwards[0]->s;
    return "";
}
bool SkipList::del(uint64_t key)
{
    Node *ncur=head;
    for(unsigned int i=random_level();i>=0;--i)
    {
        while(ncur->forwards[i]!=tail&&ncur->forwards[i]->key<key)
            ncur=ncur->forwards[i];
        if(ncur->forwards[i]!=tail&&ncur->forwards[i]->key==key)
        {
            Node *ntmp=ncur->forwards[i];
            for(;i>=0;--i)
                ncur->forwards[i]=ntmp->forwards[i];
            delete ntmp;
            return true;
        }
    }
    return false;
}
void SkipList::reset()
{
    Node *ncur=head->forwards[0];
    while(ncur!=tail)
    {
        Node *ntmp=ncur;
        ncur=ncur->forwards[0];
        delete ntmp;
    }
    for(unsigned int i=0;i<MAX_LEVEL;++i)
        head->forwards[i]=tail;
}
void SkipList::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const
{
    Node *n1=head;
    for(unsigned int i=MAX_LEVEL-1;i>=0;--i)
        while(n1->forwards[i]!=tail&&n1->forwards[i]->key<key1)
            n1=n1->forwards[i];
    Node *n2=n1;
    for(unsigned int i=MAX_LEVEL-1;i>=0;--i)
        while(n2->forwards[i]!=tail&&n2->forwards[i]->key<key2)
            n2=n2->forwards[i];
    while(n1!=n2)
    {
        list.push_back(std::make_pair(n1->key,n1->s));
        n1=n1->forwards[0];
    }
}
void SkipList::to_sstable() const
{
    //TODO
}