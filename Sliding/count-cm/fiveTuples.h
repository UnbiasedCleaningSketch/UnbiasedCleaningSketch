#ifndef _FIVETUPLES_H_
#define _FIVETUPLES_H_

#include <algorithm>
#include <set>
#include <map>
#include "BOBHash32.h"

using namespace std;


class HashDetector
{
public:
  uint32_t windowSize;
  uint64_t winStart;
  double threshold;
  map<uint64_t, Cell> hashMap;
	BOBHash32 bobhash;


	HashDetector(uint32_t _bucketNum, uint32_t _cellNum, uint32_t _windowSize)
	{
    windowSize = _windowSize;
    winStart = 0;
    threshold = 0.0;
	  bobhash.initialize(1001);
	}

	void insert(uint64_t id, uint64_t us)
	{
    while (us > winStart + (uint64_t)windowSize)
      winStart += (uint64_t)windowSize;

    pair<map<uint64_t, Cell>::iterator, bool> res = hashMap.insert(pair<uint64_t, Cell>(id, Cell(id, us, (double)us)));
    
    if (!res.second)
    {
      ++res.first->second.frequency;
      if (res.first->second.lastUS <= winStart)
      {
        ++res.first->second.window; 
        double curPeriod = (double)(us - res.first->second.lastUS); 
        res.first->second.variance += abs(curPeriod - res.first->second.recentPeriod);
        res.first->second.recentPeriod = (curPeriod + res.first->second.recentPeriod) / 2.0;
      }
      res.first->second.lastUS = us;
    }
  }

  bool query(uint64_t id)
  {
    map<uint64_t, Cell>::iterator it = hashMap.find(id);
    //printf("%d %lf %lf %lf\n", it->second.window, (double)it->second.lastUS / 60000000.0, it->second.variance, it->second.getWeight());
    return it->second.getWeight() >= threshold;
  }

  double getThreshold(int k)
  {
    set<Cell> cellSet;
    for(map<uint64_t, Cell>::iterator mapIt = hashMap.begin(); mapIt != hashMap.end(); ++mapIt)
    {
      double curPeriod = (double)(winStart + windowSize - mapIt->second.lastUS); 
      mapIt->second.variance += abs(curPeriod - mapIt->second.recentPeriod);
      mapIt->second.lastUS = winStart + windowSize;
      cellSet.insert(mapIt->second);
    }

    set<Cell>::iterator setIt = cellSet.begin();
    for(int i = 1; i < k && setIt != cellSet.end(); ++i)
    {
      Cell tmpCell = *setIt;
      printf("%d %d %lf %lf\n", tmpCell.window, tmpCell.frequency, tmpCell.variance, tmpCell.getWeight());
      ++setIt;
    }
    Cell thresCell = *setIt;
    threshold = thresCell.getWeight();
    return threshold;
  }

	
};
#endif