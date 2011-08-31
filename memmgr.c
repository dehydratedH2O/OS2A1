#include "memmgr.h"

void KernMemInit(void)
{
    //TODO: optimize thisLink/currLink junk
    void* currLink;
    PMEMPAGE thisLink;
    int i;

    printf("headOfMem: %08x\n", headOfMem);
    currLink = headOfMem;
    printf("currLink: %08x\n", currLink);
    thisLink = currLink;
    thisLink->llPages.pPrev = NULL;

    for(i = 0; i < NUMPAGES-1; i++)
    {
        printf("%i ", i);
        printf("thisLink: %08x\n", thisLink);
        thisLink->llPages.pNext = currLink + sizeof(MEMPAGE);
        printf("pNext: %08x\n", thisLink->llPages.pNext);
        thisLink->llPages.pPrev = currLink - sizeof(MEMPAGE);
        printf("pPrev: %08x\n", thisLink->llPages.pPrev);
        currLink = thisLink->llPages.pNext;
        thisLink = currLink;
    }

    thisLink->llPages.pPrev = currLink - sizeof(MEMPAGE);
    thisLink->llPages.pNext = NULL;
    printf("%i thisLink: %08x\npNext: %08x\npPrev:%08x\n",
            ++i, thisLink, thisLink->llPages.pNext, thisLink->llPages.pPrev);
    
    sizeOfLists = currLink - headOfMem;
    printf("tot size of mgmt links: %iB for %i pages\n", sizeOfLists, NUMPAGES);
    pagesUsed = sizeOfLists / PAGESIZE;
    if (sizeOfLists % PAGESIZE != 0)
        ++pagesUsed;
    printf("pages used for mgmt: %i (%iB)\n", pagesUsed,
            (pagesUsed) * PAGESIZE);

    usedPages = headOfMem;
    freePages = headOfMem + ((pagesUsed) * sizeof(MEMPAGE));
  
    printf("usedPages: %08x\n", usedPages);
    printf("freePages: %08x\n", freePages);

    PMEMPAGE lastUsed = freePages->llPages.pPrev;
    printf("lastUsed: %08x\n", freePages->llPages.pPrev);
    freePages->llPages.pPrev = NULL;
    lastUsed->llPages.pNext = NULL;

    printf("sizeof MEMPAGE: %iB\n", sizeof(MEMPAGE));

    return;
}

void* KernMemAllocPages(int n)
{

    return;
}

int KernMemFreePage(void *ptr)
{

    return 0;
}
