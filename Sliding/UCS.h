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
#include "math.h"
#include <set>
#include <unordered_map>
#include "BOBHash32.h"

class UCS
{
public:
	int32_t* counter;
	uint32_t CPB;//Ã¿¸öbucket¼¸¸öcounter 
	int N_C;//Ò»¹²¼¸¸öcounter 
	int N_B;//Ò»¹²¼¸¸öbucket 
	int N_A;//Ò»¹²¼¸¸öarray 
	int BPA;//Ã¿¸öarray¼¸¸öbucket 
	BOBHash32* hash;
	int hashnum;
	double *FT;//¸÷¸öbucketµÚÒ»´ÎÇå¿ÕµÄÊ±¼ä 
	double t; //Ö¸ÕëÉ¨Ò»È¦µÄÊ±¼ä 
	double ft;//É¨¹ýµÚÒ»¸öbucketµÄÊ±¼ä 
	double lasttime;//ÉÏ¸ö°üÀ´µÄÊ±¼ä 
	UCS(uint32_t _CPB, int _BPA, BOBHash32* _hash, int _hashnum, double _t, double _ft)
	{
		CPB=_CPB;
		N_A=_hashnum; 
		BPA=_BPA;
		hashnum=_hashnum;
		hash=_hash;
		N_C=N_A*BPA*CPB;
		lasttime=0;
		counter= new int32_t[N_C];
		memset(counter,0, N_C*sizeof(int32_t));
		N_B=N_A*BPA;


		FT= new double[N_B];
		memset(FT,0, N_B*sizeof(double));
		
		t=_t;
		ft=_ft;
		for (int i=0;i<N_B;i++)
		{
			FT[i]=ft+t/N_B*i;
		}
	}
	~UCS()
	{
		delete []counter;
		delete []FT;
	}
	void clean(double lasttime,double nowtime)
	{
		int LCC=int(lasttime/t)%CPB;
		int LBC=int64_t(lasttime/t*N_B)%N_B;
		int NCC=int(nowtime/t)%CPB;
		int NBC=int64_t(nowtime/t*N_B)%N_B;
		int LB=LBC;
		int LC=LCC;
		//cout<<LB<<"  "<<LC<<"  "<<NBC<<"  "<<NCC<<endl; 
		while (LB!=NBC||LC!=NCC)
		{
			LB++;
			if (LB==N_B)
			LB=0,LC++;
			if (LC==CPB)
			LC=0;
			counter[LB*CPB+LC]=0;
		}
	}
	void insert(uint32_t id, int32_t count, double nowtime)
	{
		clean(lasttime,nowtime);
		lasttime=nowtime;
		for (int i=0;i<hashnum;i++)
		{
			uint32_t s=hash[i].run((char*)&id, sizeof(uint32_t));
			int BC=s%BPA+i*BPA;
		
			int CC;
			if (nowtime>=FT[BC])
			{
				
				CC=int((nowtime-FT[BC])/t)%CPB;
				
			}
			else
			{
				CC=CPB-1;
			}
			//	cout<<BC<<"^^^"<<CC<<"^^^"<<endl;
			int q=((hash[i+hashnum].run((char*)&id, sizeof(uint32_t))%(1<<CPB))>>CC)&1;
			if (q==0)
			q--;
			counter[BC*CPB+CC]+=q;
		}
	}
	int query (uint32_t id, double nowtime)
	{
		int NCC=int(nowtime/t)%CPB;
		int NBC=int(nowtime/t*N_B)%N_B;

		vector<int> seq;
		for (int i=0;i<hashnum;i++)
		{
			uint32_t s=hash[i].run((char*)&id, sizeof(uint32_t));
			int BC=s%BPA+i*BPA;
			int nextClear = NCC;
			
			if (BC <= NBC)
				nextClear = (nextClear + 1) % CPB;
				
			int cnt=0;
			uint32_t Q = hash[i + hashnum].run((char *)&id, sizeof(uint32_t)) % (1 << CPB);
			int q = (Q >> nextClear) & 1;
			if (q==0)
				q--;
			cnt += round(counter[BC * CPB + nextClear] * q * (0.5 - ((nowtime - FT[BC])/t - int((nowtime - FT[BC])/t))));		
			
			for (int r=0;r<CPB;r++)
			{
				q=(Q>>r)&1;
				if (q==0)
					q--;
				cnt+=counter[BC*CPB+r]*q;//cout<<counter[BC*CPB+r]<<"  ";
			}
				//cout<<endl;
			seq.push_back(cnt);
		}
		sort(seq.begin(),seq.end());
		
		if (seq.size()%2==1)
		return seq[(seq.size()-1)/2];
		else
		return round((double)(seq[seq.size()/2]+seq[seq.size()/2-1])/2);
		/*int sum = 0;
		for (int i = 0; i < seq.size(); ++i)
			sum += seq[i];
		return round((double)sum / seq.size());*/
	}
 } ;

/*#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <bitset>
#include <time.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include "math.h"
#include "BOBHash32.h"

using namespace std;

class UCS
{
public:
	int* counter;
	uint32_t divNum; 
	uint32_t bucketNum;
	uint32_t hashNum;
	double divTime; 
	double lastTime;
	BOBHash32* hash;
	UCS(uint32_t _CPB, int _BPA, BOBHash32* _hash, int _hashnum, double _t, double _ft)
	{
		divNum = _CPB;
		hashNum = _hashnum; 
		bucketNum = _BPA;
		hash = _hash;
		lastTime = 0;
		counter = new int[divNum * bucketNum * hashNum];
		memset(counter, 0, divNum * bucketNum * hashNum * sizeof(int));
		divTime = _t;
	}

	~UCS()
	{
		delete []counter;
	}

	void clean(double lasttime,double nowtime)
	{
		int lastDiv = ((int)(lasttime / divTime)) % divNum;
		int nowDiv = ((int)(nowtime / divTime)) % divNum;
		int curDiv = lastDiv;
		while (curDiv != nowDiv)
		{
			curDiv = (curDiv + 1) % divNum;
			for (int i = 0; i < hashNum; ++i)
				for (int j = 0; j < bucketNum; ++j)
					counter[i * (bucketNum * divNum) + j * divNum + curDiv] = 0;
		}
	}

	void insert(uint32_t id, int32_t count, double nowtime)
	{
		clean(lastTime,nowtime);
		lastTime=nowtime;

		for (int i = 0; i < hashNum; ++i)
		{
			uint32_t curBucket = hash[i].run((char *)(&id), sizeof(uint32_t)) % bucketNum;
			int nowDiv = ((int)(nowtime / divTime)) % divNum;
			int inc = (hash[i + hashNum].run((char *)(&id), sizeof(uint32_t)) % 2) * 2 - 1;
			counter[i * (bucketNum * divNum) + curBucket * divNum + nowDiv] += inc;
		}
	}

	int query (uint32_t id, double nowtime)
	{
		vector<int> seq;
		for (int i = 0; i < hashNum; ++i)
		{
			uint32_t curBucket = hash[i].run((char *)(&id), sizeof(uint32_t)) % bucketNum;
			int sum = 0;
			for (int j = 0; j < divNum; ++j)
				sum += counter[i * (bucketNum * divNum) + curBucket * divNum + j];
			int inc = (hash[i + hashNum].run((char *)(&id), sizeof(uint32_t)) % 2) * 2 - 1;

			seq.push_back(sum * inc);
		}
		sort(seq.begin(), seq.end());

		if (seq.size() % 2 == 1)
			return seq[(seq.size() - 1) / 2];
		else
			return round((double)(seq[seq.size() / 2] + seq[seq.size() / 2 - 1]) / 2);
	}
 };*/

