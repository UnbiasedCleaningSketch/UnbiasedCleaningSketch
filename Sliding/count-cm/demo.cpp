#include <stdio.h>
#include <algorithm>
#include <set>
#include "HashMap.h"
#include "CMSketch.h"
#include "CSketch.h"
#include "parser.h"

using namespace std;

#define Sketch CSketch

#define LSTRING 100
char testDir[LSTRING] = "../../APT1.0/data/normal/formatted01.dat";

int main(int argc, char const *argv[])
{
	if (argc != 4)
	{
		printf("./a.out hashNum hashLen");
		return 0;
	}

	uint32_t hashNum = (uint32_t)atoi(argv[1]);
    uint32_t hashLen = (uint32_t)atoi(argv[2]);
    uint32_t dataSize = (uint32_t)atoi(argv[3]);

	FILE *ftest = fopen(testDir, "rb");	
	Packet *packet = new Packet;
	HashMap *hashMap = new HashMap();
	Sketch *sketch = new Sketch(hashNum, hashLen);
	set<uint64_t> idSet;
	int packetNum = 0;

	while (ExtractFormattedPacket(ftest, packet))
	{
		++packetNum;
		//if (packetNum % 100000 == 0)
			//printf("Reading packet %d ...\n", packetNum);
		hashMap->insert(packet->id);
		sketch->insert(packet->id);
		idSet.insert(packet->id);
		if (packetNum == dataSize)
			break;
	}

	fclose(ftest);
	
	int big = 0, size_100 = 0, size_1000 = 0, size_10000 = 0;
	double AAE = 0.0;
	double ARE = 0.0;
	int bias = 0;
	for (set<uint64_t>::iterator it = idSet.begin(); it != idSet.end(); ++it)
	{
		double actual = (double)hashMap->query(*it);
		double est = (double)sketch->query(*it);
		if (actual >= -1.0)
		{
			AAE += abs(est - actual);
			ARE += abs(est - actual) / actual;
			bias += est - actual;
			//printf("%lf %lf\n", actual, est);
			++big;
		}
		if (actual >= 100.0)
			++size_100;
		if (actual >= 1000.0)
			++size_1000;
		if (actual >= 10000.0)
			++size_10000;
	}

	AAE /= (double)big;// idSet.size();
	ARE /= (double)big;// idSet.size();


	printf("packetNum: %d\n", dataSize);
	printf("AAE: %lf\n", AAE);
	printf("ARE: %lf\n", ARE);
	printf("bias: %d\n", bias);
	//printf("100+: %d\n", size_100);
	//printf("1000+: %d\n", size_1000);
	//printf("10000+: %d\n", size_10000);

	return 0;
}
