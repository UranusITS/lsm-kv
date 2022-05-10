#include"sstable.h"
class Level
{
private:
    const uint64_t cap, filename_cnt;
    std::string pathname;
    std::list<SSInfo>ssinfos;
public:
    Level(int cap,const std::string &pathname);
    unsigned int get_cap() const;
    unsigned int get_size() const;
    bool is_overflow() const;
    void put_sstable(const SSTable &sstable);
    void put_list(const std::list<std::pair<uint64_t,std::string>>&list);
    void push_down(Level &next_level);
    void push_down_all(Level &next_level);
};
class LevelManager
{
private:
    std::vector<Level>levels;
    void add_level();
public:
    LevelManager();
    void put_sstable(const SSTable &sstable);
};
