#include "ll.h"

//following function taken from assignment instructions
void KernUnlinkLL(PLINKEDLIST *ppllHead, PLINKEDLIST pllRem)
{
    /* Make the "next pointer" of the previous node point to the node following
     * pllRem */
    if (pllRem->pPrev != NULL)
        pllRem->pPrev->pNext = pllRem->pNext;
    else
        /* If pllRem's "previous" is null then pllRem is at the head of the list,
         * which must be adjusted */
        *ppllHead = pllRem->pNext;
    /* Make the "previous pointer" of the next node point to the node preceding
     * pllRem */
    if (pllRem->pNext != NULL)
        pllRem->pNext->pPrev = pllRem->pPrev;
}
