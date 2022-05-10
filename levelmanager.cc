#include"levelmanager.h"
#include"utils.h"
const std::string Level::DELETE_LABEL="~DELETED~";
Level::Level(int cap,const std::string &pathname):cap(cap),filename_cnt(0),pathname(pathname) {}
unsigned int Level::get_cap() const
{
    return cap;
}
unsigned int Level::get_size() const
{
    return ssinfos.size();
}
bool Level::is_overflow() const
{
    return ssinfos.size()>cap;
}
void Level::put_sstable(const SSTable &sstable)
{
    std::string filename=std::to_string(filename_cnt)+".sst";
    ssinfos.push_back(SSInfo(sstable,filename));
    sstable.write(pathname+"/"+filename);
}
void Level::put_list(std::list<std::pair<uint64_t,std::string>> list)
{
    for(auto it=list.begin();it!=list.end();it++)
        if(it->second==DELETE_LABEL)
            list.erase(it);
    std::list<std::pair<uint64_t,std::string>>tmp;
    uint64_t sz=0;
    for(auto it=list.begin();it!=list.end();it++)
    {
        if(sz+13+it->second.length()>MemTable::MAX_SZ)
        {
            put_sstable(SSTable(tmp));
            tmp.clear();
            sz=0;
        }
        tmp.push_back(*it);
        sz+=13+it->second.length();
    }
    if(!tmp.empty()) put_sstable(SSTable(tmp));
}
void Level::push_down(Level &next_level)
{
    std::list<std::pair<uint64_t,std::string>>tmp;
    uint64_t del_min_key=UINT64_MAX,del_max_key=0;
    while(is_overflow())
    {
        auto del_it=ssinfos.begin();
        for(auto it=ssinfos.begin();it!=ssinfos.end();it++)
            if(it->time_stamp<del_it->time_stamp)
                del_it=it;
        SSTable sstable(del_it->filename);
        tmp.merge(sstable.get_list());
        del_min_key=std::min(del_min_key,del_it->min_key);
        del_max_key=std::max(del_max_key,del_it->max_key);
        utils::rmfile((pathname+"/"+del_it->filename).c_str());
        ssinfos.erase(del_it);
    }
    for(auto it=next_level.ssinfos.begin();it!=next_level.ssinfos.end();it++)
    {
        if(it->min_key>del_max_key||it->max_key<del_min_key)
            continue;
        SSTable sstable(it->filename);
        tmp.merge(sstable.get_list());
        utils::rmfile((next_level.pathname+"/"+it->filename).c_str());
        next_level.ssinfos.erase(it);
    }
    next_level.put_list(tmp);
}
void Level::push_down_all(Level &next_level)
{
    std::list<std::pair<uint64_t,std::string>>tmp;
    uint64_t del_min_key=UINT64_MAX,del_max_key=0;
    for(const SSInfo &ssinfo:ssinfos)
    {
        SSTable sstable(ssinfo.filename);
        tmp.merge(sstable.get_list());
        del_min_key=std::min(del_min_key,ssinfo.min_key);
        del_max_key=std::max(del_max_key,ssinfo.max_key);
        utils::rmfile((pathname+"/"+ssinfo.filename).c_str());
    }
    ssinfos.clear();
    for(auto it=next_level.ssinfos.begin();it!=next_level.ssinfos.end();it++)
    {
        if(it->min_key>del_max_key||it->max_key<del_min_key)
            continue;
        SSTable sstable(it->filename);
        tmp.merge(sstable.get_list());
        utils::rmfile((next_level.pathname+"/"+it->filename).c_str());
        next_level.ssinfos.erase(it);
    }
    next_level.put_list(tmp);
}
std::string Level::get(uint64_t key) const
{
    for(const SSInfo &ssinfo:ssinfos)
    {
        if(key<ssinfo.min_key||key>ssinfo.max_key)
            continue;
        BloomFilter bf(ssinfo.filename);
        if(bf.find(key))
        {
            SSTable sstable(ssinfo.filename);
            std::string s=sstable.get(key);
            if(s!="")
                return s;
        }
    }
    return "";
}
void Level::reset()
{
    ssinfos.clear();
    utils::rmdir(pathname.c_str());
}
void Level::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const
{
    for(const SSInfo &ssinfo:ssinfos)
    {
        if(key2<ssinfo.min_key||key1>ssinfo.max_key)
            continue;
        SSTable sstable(ssinfo.filename);
        sstable.scan(key1,key2,list);
    }
}
LevelManager::LevelManager(const std::string &pathname)
{
    utils::mkdir((pathname+"level0").c_str());
    levels.push_back(Level(2,(pathname+"level0").c_str()));
}
void LevelManager::add_level()
{
    utils::mkdir(("level"+std::to_string(levels.size())).c_str());
    levels.push_back(Level(levels[levels.size()].get_cap()*2,"level"+std::to_string(levels.size())));
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
            return s;
    }
    return "";
}
void LevelManager::reset()
{
    for(auto &level:levels)
        level.reset();
    levels.clear();
    utils::mkdir("level0");
    levels.push_back(Level(2,"level0"));
}
void LevelManager::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const
{
    for(auto &level:levels)
        level.scan(key1,key2,list);
}
