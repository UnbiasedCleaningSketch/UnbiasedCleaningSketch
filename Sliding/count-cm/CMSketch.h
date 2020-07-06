#ifndef _CMSKETCH_H_
#define _CMSKETCH_H_

#include <algorithm>
#include <memory.h>
#include "BOBHash32.h"

using namespace std;

struct Hash
{
  uint32_t hashLen;
  int *counter;
  BOBHash32 bobhash;

  Hash()
  {
  }

  ~Hash()
  {
    delete counter;
  }

  void build(uint32_t _hashLen, uint32_t seed)
  {
    hashLen = _hashLen;
    counter = new int[hashLen];
    memset(counter, 0, sizeof(counter));
    bobhash.initialize(seed);
  }

  void insert(uint64_t id, int inc)
  {
    int pos = bobhash.run((char *)&id, 8) % hashLen;
    counter[pos] += inc;
  }

  int query(uint64_t id)
  {
    int pos = bobhash.run((char *)&id, 8) % hashLen;
    return counter[pos];
  }
};

class CMSketch
{
public:
	uint32_t hashNum;
	Hash *hash;

	CMSketch(uint32_t _hashNum, uint32_t _hashLen)
	{
	  hashNum = _hashNum;
    hash = new Hash[hashNum];
    for (int i = 0; i < hashNum; ++i)
      hash[i].build(_hashLen, i);
	}

  ~CMSketch()
  {
    delete [] hash;
  }

	void insert(uint64_t id)
	{ 
		int inc = 1; //(bobhash.run((char *)&id, 8) % 2) * 2 - 1
    for (int i = 0; i < hashNum; ++i)
      hash[i].insert(id, inc);
  }

  int query(uint64_t id)
  {
    int minCount = 0x7fffffff;
    for (int i = 0; i < hashNum; ++i)
    {
      int ans = hash[i].query(id);
      if (ans < minCount)
        minCount = ans;
    }
    return minCount;
  }

};
#endif