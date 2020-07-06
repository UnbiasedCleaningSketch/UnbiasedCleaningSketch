
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <bitset>
#include <vector>
#include <algorithm>
#include <map>
#include "math.h"
#include <set>
#include <bitset>
#include "param.h"
#include "BOBHash32.h"
#include <unordered_map>

uint32_t loadTrace(vector<pair<uint32_t, uint64_t>>&vec, const char*filename, int totalflow)
{
	set<uint32_t> idset;
	uint64_t time;
	cout << "open " << filename << endl;
    FILE* pf = fopen(filename, "rb");
    if (!pf) {
        cerr << filename << "not found." << endl;
        exit(-1);
    }
    vec.clear();
    char read_key[READ_LEN];
    uint32_t ret = 0;
    while (fread(read_key, 1, READ_LEN, pf)) 
	{
     	if (ret % 2 == 0) 
		{
		
            time = *(uint64_t*)read_key;
            ret++;
        }
    	else
		{
	  
	        uint32_t key = *(uint32_t*)read_key;
	        idset.insert(key);
	        vec.push_back(pair<uint32_t,uint64_t>(key,time));
	        ret++;
	        if (idset.size() == totalflow)
	            break;
        }
    }
    fclose(pf);
    cout << "load " << ret/2 << " items, " << idset.size() << " distinct items." << endl;
    return ret / 2;
}

uint32_t loadTrace2(vector<pair<uint32_t, uint64_t>>&vec, const char*filename, int totalflow)
{
    set<uint32_t> idset;
    uint64_t time;
    cout << "open " << filename << endl;
    FILE* pf = fopen(filename, "rb");
    if (!pf) {
        cerr << filename << "not found." << endl;
        exit(-1);
    }
    vec.clear();
    char read_key[READ_LEN];
    uint32_t ret = 0;
    while (fread(read_key, 1, KEY_LEN, pf)) 
    {
        uint32_t key = *(uint32_t*)read_key;
        idset.insert(key);
        vec.push_back(pair<uint32_t,uint64_t>(key,time));
        ret++;
        if (idset.size() == totalflow)
            break;
    }
    fclose(pf);
    cout << "load " << ret << " items, " << idset.size() << " distinct items." << endl;
    return ret;
}




