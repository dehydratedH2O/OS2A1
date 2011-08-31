#include "stdlib.h"

#ifndef LL_H
#define LL_H

typedef struct LLSTRUCT
{
    struct LLSTRUCT *pPrev;
    struct LLSTRUCT *pNext;
} LINKEDLIST, *PLINKEDLIST;

#endif
