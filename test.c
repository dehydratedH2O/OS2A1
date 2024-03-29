#include "stdlib.h"
#include "stdio.h"
#include "ll.h"
#include "memmgr.c"
#include "time.h"


int main(int argc, char * argv[]) 
{
    printf("memory manager test\nRAMSIZE:\t%iB\nPAGESIZE:\t%iB\nNUMPAGES:\t%i\n",
            RAMSIZE, PAGESIZE, NUMPAGES);

    printf("testing ll.h\n");
    LINKEDLIST testLl;

    printf("testing memmgr.h\n");
    MEMPAGE testMp;

    printf("mallocing\n");
    
    //alloc RAM
    void * ramdisk = malloc(RAMSIZE);

    printf("alloc'd at %016x\n", ramdisk);

    headOfMem = ramdisk;

    printf("KernMemInit\n");
    KernMemInit();
    printf("testing memmgr.c\n");
    KernMemAllocPages(1);
    KernMemAllocPages(15);
    //alloc almost all pages
    void* testPage = KernMemAllocPages(NUMPAGES - 117);
    //try to alloc too many pages
    KernMemAllocPages(58000);
    //test freeing a page
    KernMemFreePage(testPage);
    //free enough for the next alloc, randomly
    int i, iH, iL, iToFree;
    PMEMPAGE toFree;
    srand(time(NULL));
    iH = NUMPAGES;
    iL = 0;
    for (i = 0; i < 100; i++)
    {
        iToFree = rand() % (iH - iL + 1) + iL;
        toFree = (PMEMPAGE) headOfMem + iToFree;
        KernMemFreePage((void *)toFree);
    }
    //try to alloc the num of free pages, but should fail since they're not
    //contiguous
    KernMemAllocPages(100);
    //free the RAM
    free(ramdisk);
    printf("ramdisk freed\n");

    return 0;
}
