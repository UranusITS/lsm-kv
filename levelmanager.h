#include"sstable.h"
class Level
{
private:
    const uint64_t cap;
    uint64_t filename_cnt;
    std::string pathname;
    std::list<SSInfo>ssinfos;
public:
	const static std::string DELETE_LABEL;
    Level(int cap,const std::string &pathname);
    unsigned int get_cap() const;
    unsigned int get_size() const;
    std::string get_pathname() const;
    bool is_overflow() const;
    void put_sstable(const SSTable &sstable);
    void put_list(std::list<std::pair<uint64_t,std::string>> &list);
    void push_down(Level &next_level);
    void push_down_all(Level &next_level);
    std::string get(uint64_t key) const;
    void reset();
    void sort();
    void scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const;
};
class LevelManager
{
private:
    std::string pathname;
    std::vector<Level>levels;
    void add_level();
public:
    LevelManager(const std::string &pathname=".");
    void put_sstable(const SSTable &sstable);
    std::string get(uint64_t key) const;
    void reset();
    void scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>> &list) const;
};
