#ifndef _CSKETCH_H_
#define _CSKETCH_H_

#include <algorithm>
#include <set>
#include <memory.h>
#include "CMSketch.h"
#include "BOBHash32.h"

using namespace std;

class CSketch
{
public:
  uint32_t hashNum;
  Hash *hash;
  BOBHash32 bobhash;


  CSketch(uint32_t _hashNum, uint32_t _hashLen)
  {
    hashNum = _hashNum;
    hash = new Hash[hashNum];
    for (int i = 0; i < hashNum; ++i)
      hash[i].build(_hashLen, i);
    bobhash.initialize(1001);
  }

  ~CSketch()
  {
    delete [] hash;
  }

  void insert(uint64_t id)
  { 
    int inc = (bobhash.run((char *)&id, 8) % 2) * 2 - 1;
    int midVal = (int)query(id);
    for (int i = 0; i < hashNum; ++i)
    {
      if (hash[i].query(id) == midVal)
        hash[i].insert(id, inc);
    }
  }

  double query(uint64_t id)
  {
    multiset<int> ansSet;
    double res = 0.0;

    for (int i = 0; i < hashNum; ++i)
      ansSet.insert(hash[i].query(id));

    if (hashNum % 2 == 0)
    {
      int index = hashNum / 2 - 1;
      multiset<int>::iterator it = ansSet.begin();
      while (index--)
        ++it;
      res += (double)(*it);
      res += (double)(*(++it));
      res /= 2.0;
    }
    else
    {
      int index = hashNum / 2;
      multiset<int>::iterator it = ansSet.begin();
      while (index--)
        ++it;
      res = (double)(*it);
    }

    int inc = (bobhash.run((char *)&id, 8) % 2) * 2 - 1;
    return res * inc;
  }
};
#endif