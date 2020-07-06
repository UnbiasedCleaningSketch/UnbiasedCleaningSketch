#ifndef __PARAM_H__
#define __PARAM_H__

#define KEY_LEN         4
#define READ_LEN        8

uint32_t MAXID=0xffffffff - 150;
int16_t MAXCOUNT=10000;//0x7fff;

uint64_t MAPPINGPRIME=4294967291;// max prime for 32bit numbers
uint64_t MAPPINGBOUND=((uint64_t)1<<32)+3;
uint64_t MAPPINGINVERSE=1610612737;// inverse
uint64_t MAPPINGPRIME1=4294965837;
 uint64_t MAPPINGBOUND1=((uint64_t)1<<32)+3; 
 uint64_t MAPPINGINVERSE1=1236786069;
#define PARSIZE 8

#endif
