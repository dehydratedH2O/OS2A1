#include "memmgr.h"

void KernMemInit(void)
{
    //TODO: optimize thisLink/currLink junk
    void* currLink;
    PMEMPAGE thisLink;
    int i;

#ifdef DEBUG
    printf("headOfMem: %08x\n", headOfMem);
#endif
    currLink = headOfMem;
#ifdef DEBUG
    printf("currLink: %08x\n", currLink);
#endif
    thisLink = currLink;
    //fix first page
    thisLink->llPages.pPrev = NULL;

    for(i = 0; i < NUMPAGES-1; i++)
    {
#ifdef DEBUG
        printf("%i ", i);
        printf("thisLink: %08x\n", thisLink);
#endif
        thisLink->llPages.pNext = currLink + sizeof(MEMPAGE);
#ifdef DEBUG
        printf("pNext: %08x\n", thisLink->llPages.pNext);
#endif
        thisLink->llPages.pPrev = currLink - sizeof(MEMPAGE);
#ifdef DEBUG
        printf("pPrev: %08x\n", thisLink->llPages.pPrev);
#endif
        currLink = thisLink->llPages.pNext;
        thisLink = currLink;
    }

    //fix last page
    thisLink->llPages.pPrev = currLink - sizeof(MEMPAGE);
    thisLink->llPages.pNext = NULL;
#ifdef DEBUG
    printf("%i thisLink: %08x\npNext: %08x\npPrev:%08x\n",
            ++i, thisLink, thisLink->llPages.pNext, thisLink->llPages.pPrev);
#endif
    
    sizeOfLists = currLink - headOfMem;
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
    //TODO: fix this calculation
    freePages = headOfMem + ((pagesUsed) * sizeof(MEMPAGE));
  
#ifdef DEBUG
    printf("usedPages: %08x\n", usedPages);
    printf("freePages: %08x\n", freePages);
#endif

    PMEMPAGE lastUsed = freePages->llPages.pPrev;
#ifdef DEBUG
    printf("lastUsed: %08x\n", freePages->llPages.pPrev);
#endif
    freePages->llPages.pPrev = NULL;
    lastUsed->llPages.pNext = NULL;

#ifdef DEBUG
    printf("sizeof MEMPAGE: %iB\n", sizeof(MEMPAGE));
#endif

    return;
}

void* KernMemAllocPages(int n)
{
    int contig = 0;
    PMEMPAGE currPage = freePages;
    PMEMPAGE aPage;
    PMEMPAGE yaPage;

#ifdef DEBUG
    printf("allocing %i pages\n", n);
#endif

    while (currPage->llPages.pNext != NULL && contig < n)
    {
        if (currPage->llPages.pPrev == currPage - sizeof(MEMPAGE))
            ++contig;
        currPage = currPage->llPages.pNext;
    }
    if (currPage->llPages.pNext == NULL && contig < n)
    {
        //couldn't find space
#ifdef DEBUG
        printf("currPage: %08x\n", currPage);
        printf("currPage next: %08x\ncontig: %i/%i\n",
                currPage->llPages.pNext, contig, n);
        printf("ERROR:\tcould not find %i contiguous pages\n", n);
#endif
        return NULL;
    }
    //first remove the pages from the free list
    aPage = currPage->llPages.pNext;
    yaPage = currPage - (n * sizeof(MEMPAGE));
    yaPage = yaPage->llPages.pPrev;
    aPage->llPages.pPrev = yaPage;
    yaPage->llPages.pNext = aPage;
    
    //found pages -- move them to usedPages
    //put at beginning of list to minimize instructions required
    currPage->llPages.pNext = usedPages;
    usedPages->llPages.pPrev = currPage;
    currPage = currPage - (n * sizeof(MEMPAGE));
    currPage->llPages.pPrev = NULL;
    usedPages = currPage;

#ifdef DEBUG
    printf("%i pages alloc'd\n", n);
#endif

    return (void *) usedPages;
}

int KernMemFreePage(void *ptr)
{

    return 0;
}
