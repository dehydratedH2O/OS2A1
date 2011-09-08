#include "memmgr.h"

void KernMemInit(void)
{
    PMEMPAGE currMP, prevMP, nextMP;
    int i;

#ifdef DEBUG
    printf("headOfMem: %08x\n", headOfMem);
#endif

    currMP = headOfMem;

    for(i = 0; i < NUMPAGES; i++)
    {
#ifdef DEBUG
        printf("%i ", i);
        printf("currMP: %08x\n", currMP);
#endif

        currMP->llPages.pNext = (void*) (currMP + 1);

#ifdef DEBUG
        printf("nextMP: %08x(%08x)\n", currMP->llPages.pNext, currMP + 1);
#endif

        currMP->llPages.pPrev = (void *) (currMP - 1);

#ifdef DEBUG
        printf("prevMP: %08x(%08x)\n", currMP->llPages.pPrev, currMP - 1);
#endif

        currMP = (void *) currMP->llPages.pNext;
    }

    //fix last page
    currMP->llPages.pNext = NULL;

    sizeOfLists = ((void*)(currMP + 1)) - headOfMem;

    //fix first page
    currMP = headOfMem;
    currMP->llPages.pPrev = NULL;
    
#ifdef DEBUG
    printf("tot size of mgmt links: %iB for %i pages\n", sizeOfLists, NUMPAGES);
#endif
    pagesUsed = sizeOfLists / PAGESIZE;
    if (sizeOfLists % PAGESIZE != 0)
        ++pagesUsed;
#ifdef DEBUG
    printf("pages used for mgmt: %i (%iB)\n", pagesUsed,
            (pagesUsed) * PAGESIZE);
#endif

    usedPages = headOfMem;
    freePages = headOfMem + (pagesUsed * sizeof(MEMPAGE));
  
#ifdef DEBUG
    printf("usedPages: %08x\n", usedPages);
    printf("freePages: %08x\n", freePages);
#endif

    PMEMPAGE lastUsed = (void *) freePages->llPages.pPrev;

#ifdef DEBUG
    printf("lastUsed: %08x\n", lastUsed);
    printf("sizeof MEMPAGE: %iB\n", sizeof(MEMPAGE));
#endif

    freePages->llPages.pPrev = NULL;
    lastUsed->llPages.pNext = NULL;

    return;
}

void* KernMemAllocPages(int n)
{
    int contig = 0;
    PMEMPAGE currPage = freePages;
    PMEMPAGE nextFree, prevFree;

#ifdef STATUS
    printf("allocing %i pages\n", n);
#endif
#ifdef DEBUG
    printf("freePages: %08x\n", freePages);
#endif

    do {
#ifdef DEBUG
        printf("currPage: %08x\t%i/%i\n", currPage, contig, n);
        printf("currPage prev: %08x(%08x)\n", currPage->llPages.pPrev,
                currPage - sizeof(MEMPAGE));
#endif
        if (currPage->llPages.pPrev == currPage - 1 ||
                currPage->llPages.pPrev == NULL || currPage == freePages)
            ++contig;
        else
            contig = 0;
        currPage = currPage->llPages.pNext;
    } while (currPage->llPages.pNext != NULL && contig < n);
    if (currPage->llPages.pNext == NULL && contig < n)
    {
        //couldn't find space
#ifdef DEBUG
        printf("currPage: %08x\n", currPage);
        printf("currPage next: %08x\ncontig: %i/%i\n",
                currPage->llPages.pNext, contig, n);
#endif
#ifdef STATUS
        printf("ERROR:\tcould not find %i contiguous pages\n", n);
#endif
        return NULL;
    }
    //first remove the pages from the free list
    currPage = currPage->llPages.pPrev;
    nextFree = currPage->llPages.pNext;
    prevFree = currPage - n;
#ifdef DEBUG
    printf("nextFree: %08x\n", nextFree);
    printf("prevFree: %08x\n", prevFree);
#endif
    if (prevFree > headOfMem && prevFree > freePages) {
        prevFree = prevFree->llPages.pPrev;
        nextFree->llPages.pPrev = prevFree;
        prevFree->llPages.pNext = nextFree;
    }
#ifdef DEBUG
    printf("freePages: %08x\n", freePages);
    printf("currPage(-n): %08x(%08x)\n", currPage, currPage - n);
#endif
    //if we're taking from the head of freePages, reset it
    //
    if (currPage - n + 1 == freePages)
    {
        freePages = nextFree;

#ifdef DEBUG
        printf("freePages RESET to %08x\n", freePages);
#endif
    }
    
    //TODO: fix this
    //found pages -- move them to usedPages
    //put at beginning of list to minimize instructions required
    currPage->llPages.pNext = usedPages;
    usedPages->llPages.pPrev = currPage;
    currPage = currPage - n;
    currPage->llPages.pPrev = NULL;
    usedPages = currPage;

#ifdef STATUS 
    printf("%i pages alloc'd\n", n);
#endif

    return (void *) usedPages;
}

int KernMemFreePage(void *ptr)
{
    PMEMPAGE currPage = usedPages;
    PMEMPAGE nextPage, prevPage;

#ifdef STATUS
    printf("freeing %08x\n", ptr);
#endif

#ifdef STATUS
    printf("usedPages: %08x\ncurrPage: %08x\nnext: %08x\nprev: %08x\n",
            usedPages, currPage,
            currPage->llPages.pNext, currPage->llPages.pPrev);
#endif

    while (currPage != ptr && currPage->llPages.pNext != NULL)
    {
#ifdef DEBUG
        printf("checking %08x\n", currPage);
#endif
        currPage = currPage->llPages.pNext;
    }
    if (currPage != ptr)
    {
#ifdef STATUS
        printf("ERROR: %08x not in allocated list\n", ptr);
#endif
        return 1;
    }

    //it'll never be at the tail of the list, since that's reserved

    //do this if its in the middle of the list
    prevPage = currPage->llPages.pPrev;
    nextPage = currPage->llPages.pNext;
    //if its at the head of the list
    if (prevPage == NULL)
    {
        usedPages = nextPage;
    }
    else
    {
        nextPage->llPages.pPrev = prevPage;
        prevPage->llPages.pNext = nextPage;
    }

#ifdef STATUS
    printf("%08x freed\n", ptr);
#endif
    return 0;
}
