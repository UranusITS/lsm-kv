#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include<ctime>
#include<algorithm>
#include"kvstore.h"
KVStore store("./data");
std::string gen_str()
{
    const static int STR_LEN=1024;
    std::string re;
    for(int i=0;i<STR_LEN;i++)
        re.push_back('a'+rand()%26);
    return re;
}
void gen_data_k(int *data,int n)
{
    for(int i=0;i<n;i++)
        data[i]=i;
    std::random_shuffle(data,data+n);
}
void gen_data_s(std::string *data,int n)
{
    for(int i=0;i<n;i++)
        data[i]=gen_str();
}
void test1()
{
    const int MAXN=1e4;
    int *data_kp=new int[MAXN];
    int *data_kg=new int[MAXN];
    int *data_kd=new int[MAXN];
    std::string data_s[MAXN];
    std::ofstream out("tmp");
    store.reset();
    gen_data_k(data_kp,MAXN);
    gen_data_k(data_kg,MAXN);
    for(int i=0;i<MAXN;i++)
        data_kd[i]=data_kp[i];
    gen_data_s(data_s,MAXN);
    time_t st,ed;
    for(int i=0;i<MAXN;i+=100)
    {
        st=clock();
        for(int j=i;j<i+100;j++)
            store.put(data_kp[j],data_s[j]);
        ed=clock();
        out<<ed-st<<',';
        std::cout<<"PUT TEST: "<<i<<std::endl;
        st=clock();
        for(int j=i;j<i+100;j++)
            store.get(data_kg[j]);
        ed=clock();
        out<<ed-st<<std::endl;;
        std::cout<<"GET TEST: "<<i<<std::endl;
    }
    for(int i=0;i<MAXN;i+=100)
    {
        store.reset();
        for(int j=0;j<i+100;j++)
            store.put(data_kp[j],data_s[j]);
        st=clock();
        std::random_shuffle(data_kd,data_kd+i+100);
        for(int j=0;j<100;j++)
            if(!store.del(data_kp[j]))
                std::cout<<"[ERR] del error"<<std::endl;
        ed=clock();
        out<<ed-st<<',';
        std::cout<<"DEL TEST: "<<i<<std::endl;
    }
    out<<std::endl;
}
void test2()
{
    const int MAXN=1e6;
    int *data_k=new int[MAXN];
    std::ofstream out("tmp");
    store.reset();
    gen_data_k(data_k,MAXN);
    time_t st,ed;
    for(int i=0;i<MAXN;i+=100)
    {
        st=clock();
        for(int j=i;j<i+100;j++)
        {
            std::string s=gen_str();
            store.put(data_k[j],s);
        }
        ed=clock();
        out<<ed-st<<std::endl;
        std::cout<<"PUT TEST: "<<i<<std::endl;
    }
}
int main()
{
    srand(time(nullptr));
    test1();
    test2();
    return 0;
}
