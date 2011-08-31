#include "stdlib.h"
#include "stdio.h"
#include "ll.h"
#include "memmgr.c"

int main(int argc, char * argv[]) 
{
    printf("memory manager test\nRAMSIZE:\t%iB\nPAGESIZE:\t%iB\nNUMPAGES:\t%i\n",
            RAMSIZE, PAGESIZE, NUMPAGES);

    printf("testing ll.h\n");
    LINKEDLIST testLl;

    printf("testing memmgr.h\n");
    MEMPAGE testMp;

    printf("testing memmgr.c\n");
    KernMemAllocPages(0);
    KernMemFreePage(0);

    printf("mallocing\n");
    
    //alloc RAM
    void * ramdisk = malloc(RAMSIZE);

    printf("alloc'd at %016x\n", ramdisk);

    headOfMem = ramdisk;

    printf("KernMemInit\n");
    KernMemInit();

    //free the RAM
    free(ramdisk);
    printf("ramdisk freed\n");

    return 0;
}
