#ifndef MAIN_H
#define MAIN_H

#pragma pack(1) //compiler directive to compact memory usage

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "data.h"
#include "queue.h"
#include "simulation.h"

// Forward declarations for structures used in other headers
struct rowStruct;
struct blockStruct;
struct cacheStruct;
struct argStruct;
struct varStruct;
struct resultDataStruct;
struct Queue; // Assuming Queue is used but not defined here

typedef struct argStruct {
    char* trace_file_name;
    int cache_size; //in KB
    int block_size; //in bytes
    int associativity;
    char* replacement_policy; //will be RR, RND, or LRU
} argStruct;

typedef struct varStruct {
    int total_blocks;
    int index_size; //bits
    int offset_size; //bits
    int tag_size; //bits
    int total_indices;
    int overhead_memory_size; //in bytes
    int implementation_memory_size; //in bytes

    double cache_hit_rate; //percent
    double cpi; //cycles per instruction
} varStruct;

//function prototypes

void handleIncorrectUsage(char* errorMessage);
void doCacheMath(varStruct* vars, argStruct* args);
void showCalculatedValues(varStruct* vars);
int main(int argc, char* argv[]);
bool numberIsPower(int number, int power);
double lowerPrecision(double number, int precision);

#endif