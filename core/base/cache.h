#ifndef _CACHE_
#define _CACHE_

#include <unordered_map>
#include <utility>
#include <vector>
#include <list>
#include <map>

namespace TCORE
{

template<typename KEY>
class LRU
{
	typedef std::list<KEY> KEY_LIST;
	typedef std::unordered_map<KEY, typename KEY_LIST::iterator> ITERATOR_MAP;

	size_t maxsize;
	ITERATOR_MAP iterator_map;
	KEY_LIST key_list;

public:
	LRU(size_t m) : maxsize(m) {}

	void OnFind(const KEY &key)
	{
		typename ITERATOR_MAP::iterator it = iterator_map.find(key);
		key_list.erase(it->second);
		key_list.push_front(key);
		it->second = key_list.begin();
	}

	void OnInsert(const KEY &key)
	{
		typename ITERATOR_MAP::iterator it = iterator_map.find(key);
		if(it == iterator_map.end())
		{
			key_list.push_front(key);
			iterator_map.insert(std::make_pair(key, key_list.begin()));
		}
		else
		{
			key_list.erase(it->second);
			key_list.push_front(key);
			it->second = key_list.begin();
		}
	}

	bool WashOut(KEY &key)
	{
		if(key_list.size() > maxsize)
		{
			typename KEY_LIST::reverse_iterator rit = key_list.rbegin();
			swap(key, *rit);
			key_list.pop_back();
			return true;
		}
		return false;
	}
};

template<typename KEY, typename VALUE, typename POLICY = LRU<KEY>, size_t MAXSIZE = 1024, typename CONTAINER = std::map<KEY, VALUE> >
class Cache
{
private:
	CONTAINER container;
	POLICY policy;

public:

	Cache() : policy(MAXSIZE) {}

	bool insert(const KEY &key, const VALUE &value)
	{
		std::pair<typename CONTAINER::iterator, bool> res = container.insert(key, value);
		if(res.second)
		{
			policy.OnInsert(key);
			KEY key;
			while(policy.WashOut(key))
			{
				container.erase(key);
			}
		}
		return false;
	}

	bool find(const KEY &key, VALUE &value)
	{
		typename CONTAINER::iterator it = container.find(key);
		if(it == container.end())
		{
			return false;
		}

		value = it->second;

		policy.OnFind(key);

		return true;
	}
};


}

#endif
