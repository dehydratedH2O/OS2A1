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

    while (currPage != NULL) {
#ifdef DEBUG
        printf("currPage: %08x\t%i/%i\n", currPage, contig, n);
        printf("next: %08x(%08x)\n", currPage->llPages.pNext,
                currPage + 1);
        printf("prev: %08x(%08x)\n", currPage->llPages.pPrev,
                currPage - 1);
#endif
        if (currPage->llPages.pNext == (void*) (currPage + 1))
        {
            ++contig;
            if (contig >= n)
                break;
        }
        else
            contig = 0;
        currPage = (void*) currPage->llPages.pNext;
    };
    
    if (currPage == NULL && contig < n)
    {
        //couldn't find space
#ifdef DEBUG
        printf("currPage: %08x\n", currPage);
#endif
#ifdef STATUS
        printf("ERROR:\tcould not find %i contiguous pages\n", n);
#endif
        return NULL;
    }

    //first remove the pages from the free list
    nextFree = (void*) currPage->llPages.pNext;
    prevFree = (void*) (currPage - n + 1);
    prevFree = (void*) prevFree->llPages.pPrev;

#ifdef DEBUG
    printf("nextFree: %08x\n", nextFree);
    printf("prevFree: %08x\n", prevFree);
#endif

    if (prevFree == NULL)
    {
        freePages = nextFree;
        nextFree->llPages.pPrev = NULL;
#ifdef DEBUG
        printf("freePages RESET to %08x\n", freePages);
#endif
    }
    else
    {
        nextFree->llPages.pPrev = (void*) prevFree;
        prevFree->llPages.pNext = (void*) nextFree;
    }

    //found pages -- move them to usedPages
    //put at beginning of list to minimize instructions required
    currPage->llPages.pNext = (void*) usedPages;
    usedPages->llPages.pPrev = (void*) currPage;
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
    PMEMPAGE currPage = ptr;
    PMEMPAGE nextPage, prevPage;

#ifdef STATUS
    printf("freeing %08x\n", ptr);
#endif

    //assume that it's valid and in the alloc'd list
    nextPage = (void*) currPage->llPages.pNext;
    prevPage = (void*) currPage->llPages.pPrev;

    if (nextPage == NULL)
    {
        //last page in the list
        prevPage->llPages.pNext = NULL;
    } 
    else if (prevPage == NULL)
    {
        //first page in the list
        usedPages = nextPage;
    }
    else
    {
        //middle page
        nextPage->llPages.pPrev = (void*) prevPage;
        prevPage->llPages.pNext = (void*) nextPage;
    }

    currPage->llPages.pPrev = NULL;
    currPage->llPages.pNext = (void *)freePages;
    freePages = currPage;
/*
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
*/

#ifdef STATUS
    printf("%08x freed\n", ptr);
#endif
    return 0;
}
