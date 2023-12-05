#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "queue.h"
#include "cache.h"
#include "data.h"

typedef struct cacheStruct cacheStruct;
typedef struct argStruct argStruct;
typedef struct varStruct varStruct;

typedef struct resultDataStruct {
    int cacheHits;
    int compulsoryMisses;
    int conflictMisses;
    int totalCycles;
    int totalInstructions;
    int totalCacheAccesses;
    int usedCacheBlocks;
    int numBlocksAccessed;
    float usedCacheSpace; // added to access later in main.c to calculate unused space and blocks

    int* numBlkAcsCntArry; //number of block accesses per instruction count array
    int maxNumBlkAcs; //maximum number of block accesses for all instructions
} resultDataStruct;

//function prototypes

void accessCache(cacheStruct* cache, argStruct* args, varStruct* vars, int address, int length, resultDataStruct* resDt);
void calculateUnused(cacheStruct* cache, varStruct* vars);
resultDataStruct* runSimulation(Queue* traceData, argStruct* args, varStruct* vars);

#endif