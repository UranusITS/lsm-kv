#include"kvstore.h"
#include<iostream>
KVStore::KVStore(const std::string &dir):
	KVStoreAPI(dir)
{
	memtable=new MemTable();
	level_manager=new LevelManager(dir);
}
KVStore::~KVStore()
{
	if(memtable->get_num())
		level_manager->put_sstable(SSTable(memtable));
	delete memtable;
	delete level_manager;
}
/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key,const std::string &s)
{
	//std::cout<<"PUT("<<key<<','<<s<<')'<<std::endl;
	//std::cout<<"PUT("<<key<<')'<<std::endl;
	if(!memtable->put(key,s))
	{
		level_manager->put_sstable(SSTable(memtable));
		memtable->reset();
		memtable->put(key,s);
	}
}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
	//std::cout<<"GET("<<key<<')'<<std::endl;
    std::string s=memtable->get(key);
	if(s!="")
	{
		if(s==Level::DELETE_LABEL)
			return "";
		return s;
	}
	return level_manager->get(key);
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false if the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	//std::cout<<"DEL("<<key<<')'<<std::endl;
	std::string s=memtable->get(key);
	if(s==Level::DELETE_LABEL)
		return false;
	else if(s=="")
	{
		s=level_manager->get(key);
		if(s==Level::DELETE_LABEL||s=="")
			return false;
		put(key,Level::DELETE_LABEL);
		return true;
	}
	else
		return memtable->del(key);
	return false;
}
/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	//std::cout<<"RESET"<<std::endl;
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
	//std::cout<<"SCAN("<<key1<<','<<key2<<')'<<std::endl;
	memtable->scan(key1,key2,list);
	level_manager->scan(key1,key2,list);
	for(auto it=list.begin();it!=list.end();it++)
	{
		if(it->second==Level::DELETE_LABEL)
			list.erase(it);
	}
}
