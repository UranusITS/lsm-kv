#include "kvstore.h"
KVStore::KVStore(const std::string &dir):
	KVStoreAPI(dir)
{
	memtable=new MemTable();
	level_manager=new LevelManager(dir);
}
KVStore::~KVStore()
{
	level_manager->put_sstable(SSTable(*memtable));
	delete memtable;
	delete level_manager;
}
/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key,const std::string &s)
{
	if(!memtable->put(key,s))
	{
		level_manager->put_sstable(SSTable(*memtable));
		memtable->reset();
	}
	memtable->put(key,s);
}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
    std::string s=memtable->get(key);
	if(s!="")
		return s;
	return level_manager->get(key);
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false if the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	if(memtable->del(key)) return true;
	else if(level_manager->get(key)!="")
		put(key,Level::DELETE_LABEL);
}
/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	memtable->reset();
	level_manager->reset();
}
/**
 * Return a list including all the key-value pair between key1 and key2.
 * keys in the list should be in an ascending order.
 * An empty string indicates not found.
 */
void KVStore::scan(uint64_t key1,uint64_t key2,std::list<std::pair<uint64_t,std::string>>&list)
{
	memtable->scan(key1,key2,list);
	level_manager->scan(key1,key2,list);
	for(auto it=list.begin();it!=list.end();it++)
		if(it->second==Level::DELETE_LABEL)
			list.erase(it);
}
