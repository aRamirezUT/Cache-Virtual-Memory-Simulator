#include "cache.h"

cacheStruct* initializeCache(int totalIndices, int associativity) {
    cacheStruct* cache = (cacheStruct*) malloc(sizeof(cacheStruct));
    if(cache == NULL) {
        printf("error: failed to malloc cache\n");
        exit(-1);
    }
    cache->firstRow = NULL;
    
    int i, j;
    bool firstRow = true, firstBlock;
    blockStruct *block, *lastBlock;
    rowStruct *row, *lastRow;

    //initialize rows
    for(i=0; i < totalIndices; i++) {
        row = (rowStruct*) malloc(sizeof(rowStruct));
        if(row == NULL) {
            printf("error: failed to malloc row %d\n", i);
            freeCache(cache);
            exit(-1);
        }
        row->firstBlock = NULL;
        row->nextRow = NULL;
        row->lastUsedIndex = -1;
        if(firstRow) {
            cache->firstRow = row;
            firstRow = false;
        } else {
            lastRow->nextRow = row;
        }
        lastRow = row;

        //initialize blocks
        firstBlock = true;
        for(j=0; j < associativity; j++) {
            block = (blockStruct*) malloc(sizeof(blockStruct));
            if(block == NULL) {
                printf("error: failed to malloc row %d, block %d\n", i, j);
                freeCache(cache);
                exit(-1);
            }
            block->nextBlock = NULL;
            block->timeSinceLastUse = 0;
            block->valid = false;
            block->tag = 0;
            if(firstBlock) {
                row->firstBlock = block;
                firstBlock = false;
            } else {
                lastBlock->nextBlock = block;
            }
            lastBlock = block;
        }
    }

    return cache;
}

rowStruct* getRowByIndex(cacheStruct* cache, int index) {
    if(cache->firstRow == NULL) {
        printf("error: cache contains no rows\n");
        return NULL;
    }
    int i;
    rowStruct* current = cache->firstRow;
    for(i=0; i < index; i++) {
        current = current->nextRow;
    }
    return current;
}

blockStruct* getBlockByIndex(rowStruct* row, int index) {
    if(row->firstBlock == NULL) {
        printf("error: row contains no blocks\n");
        return NULL;
    }
    int i;
    blockStruct* current = row->firstBlock;
    for(i=0; i < index; i++) {
        current = current->nextBlock;
    }
    return current;
}

blockStruct* getBlockByTag(rowStruct* row, int tag, int associativity) {
    if(row->firstBlock == NULL) {
        printf("error: row contains no blocks\n");
        return NULL;
    }
    int i;
    blockStruct* current = row->firstBlock;
    for(i=0; i < associativity; i++) {
        if(current->tag == tag) return current;
        current = current->nextBlock;
    }
    return NULL;
}

blockStruct* getLRUBlock(rowStruct* row) {
    int numLRU = 0;
    blockStruct *current = row->firstBlock, *blockLRU = row->firstBlock;
    if(current == NULL) return NULL;
    while(current != NULL) {
        if(current->timeSinceLastUse > numLRU) {
            numLRU = current->timeSinceLastUse;
            blockLRU = current;
        }
        current = current->nextBlock;
    }
    return blockLRU;
}

void addOneTimeToAll(rowStruct* row) {
    if(row == NULL || row->firstBlock == NULL) return;
    blockStruct* current = row->firstBlock;
    while(current != NULL) {
        current->timeSinceLastUse++;
        current = current->nextBlock;
    }
}

void freeCache(cacheStruct* cache) {
    rowStruct *currentRow, *tmpRow;
    blockStruct *currentBlock, *tmpBlock;
    
    currentRow = cache->firstRow;
    while(currentRow != NULL) {
        currentBlock = currentRow->firstBlock;
        while(currentBlock != NULL) {
            tmpBlock = currentBlock;
            currentBlock = currentBlock->nextBlock;
            free(tmpBlock);
        }
        tmpRow = currentRow;
        currentRow = currentRow->nextRow;
        free(tmpRow);
    }

    free(cache);
}