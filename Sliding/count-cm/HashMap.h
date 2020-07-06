#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include <algorithm>
#include <set>
#include <map>
#include "BOBHash32.h"

using namespace std;


class HashMap
{
public:
  map<uint64_t, int> hashMap;

	HashMap()
	{

	}

	void insert(uint64_t id)
	{
    pair<map<uint64_t, int>::iterator, bool> ret = hashMap.insert(pair<uint64_t, int>(id, 1));
    if (!ret.second)
      ++ret.first->second;
  }

  int query(uint64_t id)
  {
    map<uint64_t, int>::iterator it = hashMap.find(id);
    if (it != hashMap.end())
      return it->second;
    else
      return 0;
  }
	
};
#endif