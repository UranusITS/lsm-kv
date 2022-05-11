#include"levelmanager.h"
#include"utils.h"
#include<vector>
#include<iostream>
const std::string Level::DELETE_LABEL="~DELETED~";
Level::Level(int cap,const std::string &pathname):
    cap(cap),filename_cnt(0),pathname(pathname)
{
    std::vector<std::string>files;
    utils::scanDir(pathname,files);
    for(auto &file:files)
    {
        SSInfo ssinfo=SSInfo(pathname+"/"+file,file);
        ssinfos.push_back(ssinfo);
        filename_cnt=std::max(filename_cnt,uint64_t(atoll(file.c_str())+1));
    }
    sort();
}
unsigned int Level::get_cap() const
{
    return cap;
}
unsigned int Level::get_size() const
{
    return ssinfos.size();
}
std::string Level::get_pathname() const
{
    return pathname;
}
bool Level::is_overflow() const
{
    return ssinfos.size()>cap;
}
void Level::put_sstable(const SSTable &sstable)
{
    std::string filename=std::to_string(filename_cnt++)+".sst";
    ssinfos.push_back(SSInfo(sstable,filename));
    sstable.write(pathname+"/"+filename);
    sort();
}
void Level::put_list(std::list<std::pair<uint64_t,std::string>> &list)
{
    for(auto it=list.begin();it!=list.end();)
    {
        if(it->second==DELETE_LABEL)
            list.erase(it++);
        else
            it++;
    }
    std::list<std::pair<uint64_t,std::string>>tmp;
    uint64_t sz=MemTable::INIT_SZ;
    for(auto it=list.begin();it!=list.end();it++)
    {
        if(sz+13+it->second.length()>MemTable::MAX_SZ)
        {
            put_sstable(SSTable(tmp));
            tmp.clear();
            sz=MemTable::INIT_SZ;
        }
        tmp.push_back(*it);
        sz+=13+it->second.length();
    }
    if(!tmp.empty()) put_sstable(SSTable(tmp));
    sort();
}
void Level::push_down(Level &next_level)
{
    std::list<std::pair<uint64_t,std::string>>re;
    uint64_t del_min_key=UINT64_MAX,del_max_key=0;
    while(is_overflow())
    {
        auto del_it=ssinfos.begin();
        for(auto it=ssinfos.begin();it!=ssinfos.end();it++)
            if(it->time_stamp<del_it->time_stamp)
                del_it=it;
        SSTable sstable(pathname+"/"+del_it->filename);
        std::list<std::pair<uint64_t,std::string>>tmp=sstable.get_list();
        auto rit=re.begin();
        for(auto tit=tmp.begin();tit!=tmp.end();tit++)
        {
            while(rit!=re.end()&&tit->first>rit->first)
                rit++;
            if(rit!=re.end()&&tit->first==rit->first)
                continue;
            re.insert(rit,*tit);
        }
        del_min_key=std::min(del_min_key,del_it->min_key);
        del_max_key=std::max(del_max_key,del_it->max_key);
        utils::rmfile((pathname+"/"+del_it->filename).c_str());
        ssinfos.erase(del_it);
    }
    for(auto it=next_level.ssinfos.begin();it!=next_level.ssinfos.end();)
    {
        if(it->min_key>del_max_key||it->max_key<del_min_key)
        {
            it++;
            continue;
        }
        SSTable sstable(next_level.pathname+"/"+it->filename);
        std::list<std::pair<uint64_t,std::string>>tmp=sstable.get_list();
        auto rit=re.begin();
        for(auto tit=tmp.begin();tit!=tmp.end();tit++)
        {
            while(rit!=re.end()&&tit->first>rit->first)
                rit++;
            if(rit!=re.end()&&tit->first==rit->first)
                continue;
            re.insert(rit,*tit);
        }
        utils::rmfile((next_level.pathname+"/"+it->filename).c_str());
        next_level.ssinfos.erase(it++);
    }
    next_level.put_list(re);
    sort();
}
void Level::push_down_all(Level &next_level)
{
    std::list<std::pair<uint64_t,std::string>>re;
    uint64_t del_min_key=UINT64_MAX,del_max_key=0;
    for(const SSInfo &ssinfo:ssinfos)
    {
        SSTable sstable(pathname+"/"+ssinfo.filename);
        std::list<std::pair<uint64_t,std::string>>tmp=sstable.get_list();
        auto rit=re.begin();
        for(auto tit=tmp.begin();tit!=tmp.end();tit++)
        {
            while(rit!=re.end()&&tit->first>rit->first)
                rit++;
            if(rit!=re.end()&&tit->first==rit->first)
                continue;
            re.insert(rit,*tit);
        }
        del_min_key=std::min(del_min_key,ssinfo.min_key);
        del_max_key=std::max(del_max_key,ssinfo.max_key);
        utils::rmfile((pathname+"/"+ssinfo.filename).c_str());
    }
    ssinfos.clear();
    for(auto it=next_level.ssinfos.begin();it!=next_level.ssinfos.end();)
    {
        if(it->min_key>del_max_key||it->max_key<del_min_key)
        {
            it++;
            continue;
        }
        SSTable sstable(next_level.pathname+"/"+it->filename);
        std::list<std::pair<uint64_t,std::string>>tmp=sstable.get_list();
        auto rit=re.begin();
        for(auto tit=tmp.begin();tit!=tmp.end();tit++)
        {
            while(rit!=re.end()&&tit->first>rit->first)
                rit++;
            if(rit!=re.end()&&tit->first==rit->first)
                continue;
            re.insert(rit,*tit);
        }
        utils::rmfile((next_level.pathname+"/"+it->filename).c_str());
        next_level.ssinfos.erase(it++);
    }
    next_level.put_list(re);
}
std::string Level::get(uint64_t key) const
{
    for(const SSInfo &ssinfo:ssinfos)
    {
        if(key<ssinfo.min_key||key>ssinfo.max_key)
            continue;
        BloomFilter bf(pathname+"/"+ssinfo.filename);
        //std::cout<<"load "<<(pathname+"/"+ssinfo.filename)<<std::endl;
        if(bf.find(key))
        {
            SSTable sstable(pathname+"/"+ssinfo.filename);
            std::string s=sstable.get(key);
            if(s!="")
                return s;
        }
    }
    return "";
}
void Level::reset()
{
    for(auto &ssinfo:ssinfos)
        utils::rmfile((pathname+"/"+ssinfo.filename).c_str());
    ssinfos.clear();
}
void Level::sort()
{
    ssinfos.sort();
}
void Level::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const
{
    for(const SSInfo &ssinfo:ssinfos)
    {
        if(key2<ssinfo.min_key||key1>ssinfo.max_key)
            continue;
        SSTable sstable(pathname+"/"+ssinfo.filename);
        sstable.scan(key1,key2,list);
    }
}
LevelManager::LevelManager(const std::string &pathname):
    pathname(pathname)
{
    std::vector<std::string>dirs;
    utils::scanDir(pathname,dirs);
    if(!dirs.size())
    {
        std::string level_pathname=pathname+"/level0";
        if(!utils::dirExists(level_pathname))
            utils::mkdir(level_pathname.c_str());
        levels.push_back(Level(2,level_pathname));
        return ;
    }
    for(auto &dir:dirs)
    {
        //std::cout<<dir<<std::endl;
        std::string level_pathname=pathname+"/"+dir;
        if(levels.size())
            levels.push_back(Level(levels[levels.size()-1].get_cap()*2,level_pathname));
        else
            levels.push_back(Level(2,level_pathname));
    }
}
void LevelManager::add_level()
{
    std::string level_pathname=pathname+"/level"+std::to_string(levels.size());
    if(!utils::dirExists(level_pathname))
        utils::mkdir(level_pathname.c_str());
    levels.push_back(Level(levels[levels.size()-1].get_cap()*2,level_pathname));
}
void LevelManager::put_sstable(const SSTable &sstable)
{
    levels[0].put_sstable(sstable);
    if(levels[0].is_overflow())
    {
        if(levels.size()==1) add_level();
        levels[0].push_down_all(levels[1]);
    }
    for(unsigned int i=1;i<levels.size()&&levels[i].is_overflow();i++)
    {
        if(levels.size()==i+1) add_level();
        levels[i].push_down(levels[i+1]);
    }
}
std::string LevelManager::get(uint64_t key) const
{
    for(auto &level:levels)
    {
        std::string s=level.get(key);
        if(s!="")
        {
            if(s==Level::DELETE_LABEL)
                return "";
            return s;
        }
    }
    return "";
}
void LevelManager::reset()
{
    for(auto &level:levels)
    {
        level.reset();
        utils::rmdir(level.get_pathname().c_str());
    }
    levels.clear();
    utils::mkdir((pathname+"/level0").c_str());
    levels.push_back(Level(2,pathname+"/level0"));
}
void LevelManager::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const
{
    for(auto &level:levels)
        level.scan(key1,key2,list);
}
