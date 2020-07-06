#include <iostream>
using namespace std;
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <bitset>
#include <time.h>
#include <vector>
#include <algorithm>
#include <map>
#include <sys/time.h>
#include "math.h"
#include <set>
#include <unordered_map>
#include "BOBHash32.h"
BOBHash32 hashes[1000];
#include "trace.h"
#include "UCS.h"
ofstream fout;

#define SPEED 1
uint32_t CheckPoint;
uint32_t TOPK;
vector<int> randIndex;
vector<uint32_t> idVec;
int cnt(vector<pair<uint32_t, uint64_t>>&vec, double nowtime, double T, uint32_t idToQuery)
{
	int r=0; 
	for (int i=nowtime*SPEED;i>max(-1,int((nowtime-T)*SPEED));i--)
	{
		if (vec[i].first==idToQuery)
		r++;
	}
	return r;
}
void testerror(BOBHash32* _hash,int _hashnum, uint32_t _CPB, int _BPA,double _T, double _ft,char* filename,int _totalflow)
{
	
	double now=0;
	vector<pair<uint32_t, uint64_t>> vec;
	uint32_t packetNum = loadTrace2(vec,filename,_totalflow);
	//CheckPoint = rand() % packetNum;

	UCS ucs(_CPB, _BPA,  _hash,  _hashnum,  _T/(_CPB-0.5), _ft);
	printf("hashNum: %d  counter: %d  bucket: %d  mem: %dKB  width: %d  CheckPoint: %d  TOPK:%d\n", 
		_hashnum, _CPB, _BPA, _hashnum * _CPB * _BPA * 4 / 1000, (int)_T, CheckPoint, TOPK);

	set<uint32_t> idSet;
	for (int i=0;i<vec.size();i++)
	{
		if ((idSet.insert(vec[i].first)).second)
			idVec.push_back(vec[i].first);	
	}

	for (int i=0;i<vec.size();i++)
		randIndex.push_back(rand() % idVec.size());	

	double aae = 0, are = 0, variance = 0, bias = 0;
	double aae_s = 0, aae_m = 0, aae_l = 0;
	double are_s = 0, are_m = 0, are_l = 0;
	double variance_s = 0, variance_m = 0, variance_l = 0;
	int size_s = 0, size_m = 0, size_l = 0;
	double aae_topk = 0, are_topk = 0, variance_topk = 0, bias_topk = 0;
	int size_topk = 0;
	int hit = 0, all = 0, report = 0;

	double insertTime = 0.0;
	timeval startTime, endTime;

	for (int i=0;i<vec.size();i++)
	{
		if (i == 10000)
			printf("Reading packet %d ...\n", i);
		else if (i % 10000 == 0 && i > 0)
			printf("\033[1A\033[KReading packet %d ...\n", i);
		now=double(i)/SPEED;
		//cout<<"yes"<<endl;
		gettimeofday(&startTime, NULL);
		ucs.insert(vec[i].first,1,now);
		gettimeofday(&endTime, NULL);
		insertTime += ((double)endTime.tv_sec * 1000000 + (double)endTime.tv_usec) - ((double)startTime.tv_sec * 1000000 + (double)startTime.tv_usec);
		

		if (i != CheckPoint)
			continue;
		
		multiset<int> actualFreqMultiSet, estmtFreqMultiSet;
		for (int j = 0; j < idSet.size(); ++j)
		{
			double actual = (double)cnt(vec, now, _T, idVec[j]);
			double estmt = (double)ucs.query(idVec[j], now);
			if (actual == 0)
				continue;
			/*if (estmt < actual)
				printf("-");
			else if (estmt == actual)
				printf(" ");
			else
				printf("+");*/
			actualFreqMultiSet.insert(actual);
			estmtFreqMultiSet.insert(estmt);

			aae+=abs(estmt-actual);
			are+=abs((estmt-actual)/actual);
			variance+=(estmt-actual)*(estmt-actual);
			bias += estmt - actual;
			
			if (actual < _T * 0.005)
			{
				++size_s;
				aae_s += abs(estmt-actual);
				are_s += abs((estmt-actual)/actual);
				variance_s += (estmt-actual)*(estmt-actual);
			}
			else if (actual < _T)
			{
				++size_m;
				aae_m += abs(estmt-actual);
				are_m += abs((estmt-actual)/actual);
				variance_m += (estmt-actual)*(estmt-actual);
			}
			else
			{
				++size_l;
				aae_l += abs(estmt-actual);
				are_l += abs((estmt-actual)/actual);
				variance_l += (estmt-actual)*(estmt-actual);
			}
		}
		
		multiset<int>::iterator actualFreqMultiSetIt = actualFreqMultiSet.end();
		multiset<int>::iterator estmtFreqMultiSetIt = estmtFreqMultiSet.end();
		int k = TOPK;
		while (k--)
		{
			--actualFreqMultiSetIt;
			--estmtFreqMultiSetIt;
		}

		for (int j = 0; j < idSet.size(); ++j)
		{
			double actual_topk = (double)cnt(vec, now, _T, idVec[j]);
			double estmt_topk = (double)ucs.query(idVec[j], now);
			if (actual_topk >= *actualFreqMultiSetIt)
				++all;
			if (estmt_topk >= *estmtFreqMultiSetIt)
				++report;
			if (actual_topk >= *actualFreqMultiSetIt && estmt_topk >= *estmtFreqMultiSetIt)
				++hit;
			if (actual_topk < *actualFreqMultiSetIt)
				continue;
			++size_topk;
			aae_topk += abs(estmt_topk - actual_topk);
			are_topk += abs((estmt_topk - actual_topk) / actual_topk);
			variance_topk += (estmt_topk - actual_topk) * (estmt_topk - actual_topk);
			bias_topk += estmt_topk - actual_topk;
		}		

		aae/=(size_s+size_m+size_l);
		are/=(size_s+size_m+size_l);
		variance/=(size_s+size_m+size_l);
		aae_s /= size_s;
		are_s /= size_s;
		variance_s /= size_s;
		aae_m /= size_m;
		are_m /= size_m;
		variance_m /= size_m;
		aae_l /= size_l;
		are_l /= size_l;
		variance_l /= size_l;
		aae_topk /= size_topk;
		are_topk /= size_topk;
		variance_topk /= size_topk;
	}
	

	printf("All- size:%10d|aae:%10lf|are:%10lf|var:%10lf|bias:%10lf\n", (size_s+size_m+size_l), aae, are, variance, bias);
	printf("  S- size:%10d|aae:%10lf|are:%10lf|var:%10lf\n", size_s, aae_s, are_s, variance_s);
	printf("  M- size:%10d|aae:%10lf|are:%10lf|var:%10lf\n", size_m, aae_m, are_m, variance_m);
	printf("  L- size:%10d|aae:%10lf|are:%10lf|var:%10lf\n", size_l, aae_l, are_l, variance_l);
	printf("tpK- size:%10d|aae:%10lf|are:%10lf|var:%10lf|pre:%10lf|rec%10lf\n", size_topk, aae_topk, are_topk, variance_topk, (double)hit/(double)report, (double)hit/(double)all);
	printf("Mips: %lf\n\n", vec.size() / insertTime);
}


void testtime(BOBHash32* _hash,int _hashnum, uint32_t _CPB, int _BPA,double _T, double _ft,char* filename,int _totalflow)
{
	
	double now=0;
	vector<pair<uint32_t, uint64_t>> vec;
	uint32_t packetNum = loadTrace(vec,filename,_totalflow);
	//CheckPoint = rand() % packetNum;

	UCS ucs(_CPB, _BPA,  _hash,  _hashnum,  _T/(_CPB-0.5), _ft);
	printf("hashNum: %d  counter: %d  bucket: %d  mem: %dKB  width: %d  CheckPoint: %d  TOPK:%d\n", 
		_hashnum, _CPB, _BPA, _hashnum * _CPB * _BPA * 4 / 1000, (int)_T, CheckPoint, TOPK);

	set<uint32_t> idSet;
	for (int i=0;i<vec.size();i++)
	{
		if ((idSet.insert(vec[i].first)).second)
			idVec.push_back(vec[i].first);	
	}

	for (int i=0;i<vec.size();i++)
		randIndex.push_back(rand() % idVec.size());	

	double insertTime = 0.0;
	timeval startTime, endTime;
	
	gettimeofday(&startTime, NULL);
	for (int i=0;i<vec.size();i++)
	{
		now=double(i)/SPEED;
		//cout<<"yes"<<endl;
		
		ucs.insert(vec[i].first,1,now);
	}
	gettimeofday(&endTime, NULL);
	insertTime += ((double)endTime.tv_sec * 1000000 + (double)endTime.tv_usec) - ((double)startTime.tv_sec * 1000000 + (double)startTime.tv_usec);
	
	
	printf("Mips: %lf\n\n", vec.size() / insertTime);
}

int main(int argc, char const *argv[])
{
	uint32_t hashNum = (uint32_t)atoi(argv[1]);
	uint32_t CPB = (uint32_t)atoi(argv[2]);
    uint32_t BPA = (uint32_t)atoi(argv[3]);
    double width = (double)atoi(argv[4]);
    CheckPoint = (uint32_t)atoi(argv[5]);
    TOPK = (uint32_t)atoi(argv[6]);
    srand(int(time(0)));

	//fout.open("../APT1.0/data/normal/formatted01.dat");
    set<int>seeds;
   	for(int i=0;i<100;i++)
    {
        uint32_t seed = rand()%MAX_PRIME32;
        while(seeds.find(seed)!=seeds.end())
            seed = rand()%MAX_PRIME32;
        seeds.insert(seed);
        hashes[i].initialize(seed);
    }
    testerror(hashes,hashNum,CPB,BPA,width,0,"/usr/share/dataset/DataSet/new_zipf/006.dat",100000);
	//testtime(hashes,hashNum,CPB,BPA,width,0,"../APT1.0/data/normal/formatted01.dat",100000);
	
 } 