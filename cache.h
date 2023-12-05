#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include "main.h"

typedef struct rowStruct rowStruct;
typedef struct blockStruct blockStruct;

typedef struct cacheStruct {
    rowStruct* firstRow;
} cacheStruct;

typedef struct rowStruct {
    blockStruct* firstBlock;
    rowStruct* nextRow;
    int lastUsedIndex;
} rowStruct;

typedef struct blockStruct {
    blockStruct* nextBlock;
    int timeSinceLastUse;
    bool valid;
    int tag;
} blockStruct;

//function prototypes

cacheStruct* initializeCache(int totalIndices, int associativity);
rowStruct* getRowByIndex(cacheStruct* cache, int index);
blockStruct* getBlockByIndex(rowStruct* row, int index);
blockStruct* getBlockByTag(rowStruct* row, int tag, int associativity);
blockStruct* getLRUBlock(rowStruct* row);
void addOneTimeToAll(rowStruct* row);
void freeCache(cacheStruct* cache);

#endif