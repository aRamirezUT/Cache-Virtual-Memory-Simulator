#include "simulation.h"

void accessCache(cacheStruct* cache, argStruct* args, varStruct* vars, int address, int bytesRead, resultDataStruct* resDt) {
    // [     TAG      |  INDEX  | OFFSET ]
    int tag = address >> (vars->index_size + vars->offset_size);
    int index = (address & (0xFFFFFFFF >> (32 - (vars->index_size + vars->offset_size)))) >> vars->offset_size;
    int offset = address & (0xFFFFFFFF >> (32 - (vars->offset_size)));

    int i;
    int numBlocksAccessed = (int) ceil( (double) (offset + bytesRead)/args->block_size );

    int diff = numBlocksAccessed - (resDt->maxNumBlkAcs - 1); //(account for zero-index)
    if(diff > 0) {
        resDt->numBlkAcsCntArry = (int*) realloc(resDt->numBlkAcsCntArry, sizeof(int)*(resDt->maxNumBlkAcs + diff));
        for(i=0; i < diff; i++) resDt->numBlkAcsCntArry[resDt->maxNumBlkAcs + i] = 0;
        resDt->maxNumBlkAcs += diff;
    }
    resDt->numBlkAcsCntArry[numBlocksAccessed]++;

    int indexStarting = index;
    for(i=0; i < numBlocksAccessed; i++) {
        rowStruct* row = getRowByIndex(cache, index - 1);
        if(row == NULL) {
            printf("ERROR: cache overflow, row at zero-indexed %d was null\n", index - 1);
            printf("Tried reading %d times past index %d.\n", numBlocksAccessed - 1, indexStarting);
            printf("Skipping remaining reads/writes for cache access.\n\n");
            //keep results as accurate as possible
            resDt->totalCycles += numBlocksAccessed * 2; //average of hit and miss
            resDt->totalCacheAccesses += numBlocksAccessed;
            break;
        }
        blockStruct* block = getBlockByTag(row, tag, args->associativity);

        //check for hit
        if(block != NULL && block->valid == true) { //hit
            resDt->cacheHits++;
            resDt->totalCycles += 1;
        } else { //miss
            //pick a block for replacement
            if(strcmp(args->replacement_policy, "RR") == 0) {
                if(row->lastUsedIndex == -1 || row->lastUsedIndex + 1 == args->associativity) {
                    block = row->firstBlock;
                    row->lastUsedIndex = 0;
                } else {
                    block = getBlockByIndex(row, row->lastUsedIndex)->nextBlock;
                    row->lastUsedIndex++;
                }
            } else if(strcmp(args->replacement_policy, "RND") == 0) {
                block = getBlockByIndex(row, rand() % args->associativity);
            }
            if(block->valid == false) resDt->compulsoryMisses++;
            else resDt->conflictMisses++;
            block->tag = tag;
            block->valid = true;
            resDt->totalCycles += 3;
        }
        resDt->totalCacheAccesses++;
        index += 1;
    }
    // This assumes a block is considered used if it is valid, Calculate used cache space within accessCache method
    int usedBlocks = 0;
    rowStruct* currentRow = cache->firstRow;

    // Traverse the rows
    while (currentRow != NULL) {
        blockStruct* currentBlock = currentRow->firstBlock;

        // Traverse the blocks in each row
        while (currentBlock != NULL) {
            if (currentBlock->valid) {
                usedBlocks++;
            }
            currentBlock = currentBlock->nextBlock;
        }

        currentRow = currentRow->nextRow;
    }

    // Calculate used space in bytes
    float usedSpaceBytes = usedBlocks * args->block_size;
    resDt->usedCacheSpace = usedSpaceBytes; // Update the used cache space variable
    resDt->usedCacheBlocks = usedBlocks;
}

resultDataStruct* runSimulation(Queue* traceData, argStruct* args, varStruct* vars) {
    //initialize cache
    cacheStruct* cache = initializeCache(vars->total_indices, args->associativity);
    int i;
    traceItem* item;

    resultDataStruct* resDt = (resultDataStruct*) malloc(sizeof(resultDataStruct));
    resDt->cacheHits = 0, resDt->compulsoryMisses = 0, resDt->conflictMisses = 0;
    resDt->totalCycles = 0, resDt->totalInstructions = 0, resDt->totalCacheAccesses = 0, resDt->numBlocksAccessed = 0;

    // for number of blocks accessed per instruction statistics
    resDt->maxNumBlkAcs = 2 +1; //maximum number of block accesses for all instructions
                                //+1 for zero-index
    resDt->numBlkAcsCntArry = (int*) malloc(sizeof(int)*resDt->maxNumBlkAcs); //number of block accesses per instruction count
                                                                              //(i.e. numBlkAcsCnt[1] = 5, 1 block access per instruction occured 5 times)
    for(i=0; i < resDt->maxNumBlkAcs; i++) resDt->numBlkAcsCntArry[i] = 0;

    //process instructions
    while(!isEmpty(traceData)) {
        item = (traceItem*) dequeue(traceData);

        accessCache(cache, args, vars, item->addrOfInstr, item->lenOfInstr, resDt); // instruction access
        if(item->srcM != 0) accessCache(cache, args, vars, item->srcM, 4, resDt); // read data, assuming 4 bytes for all data accesses
        if(item->dstM != 0) accessCache(cache, args, vars, item->dstM, 4, resDt); // write data, assuming 4 bytes for all data accesses

        if(item->srcM != 0 || item->dstM != 0) resDt->totalCycles += 2;
        resDt->totalCycles += 2;
        resDt->totalInstructions++;
        free(item);
    }

    //show number of blocks accessed per instruction statistics
    for(i=1; i < resDt->maxNumBlkAcs; i++) {
        // printf("%d block%s accessed %d time%s in a single instruction\n", 
            // i, (i != 1 ? "s" : ""), resDt->numBlkAcsCntArry[i], (resDt->numBlkAcsCntArry[i] != 1 ? "s" : ""));
            resDt->numBlocksAccessed += resDt->numBlkAcsCntArry[i];
}
    // printf("\n");

    //save results
    vars->cache_hit_rate = (double) resDt->cacheHits/resDt->totalCacheAccesses * 100;
    vars->cpi = (double) resDt->totalCycles/resDt->totalInstructions;
    freeCache(cache);
    freeQueue(traceData);
    return resDt;
}