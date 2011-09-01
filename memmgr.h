#include "stdlib.h"
#include "ll.h"

#ifndef MEMMGR_H
#define MEMMGR_H

//size of RAM to malloc. I use 64MB since I'm in a VM with 512MB
#define RAMSIZE 65536000
#define PAGESIZE 4096
#define NUMPAGES RAMSIZE/PAGESIZE

#define DEBUG

unsigned int sizeOfLists = 0;
unsigned int pagesUsed = 0;

typedef struct
{
        LINKEDLIST llPages;
        unsigned int data;
} MEMPAGE, *PMEMPAGE;

//this changes for testing on my PC
void* headOfMem = 0;

PMEMPAGE usedPages = 0;
PMEMPAGE freePages = 0;

void KernMemInit(void);
void* KernMemAllocPages(int n);
int KernMemFreePage(void *ptr);

#endif
