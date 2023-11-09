#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <semaphore.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

typedef struct cache_block {
    int valid;
    unsigned int tag;
    unsigned char data[64];
} cache_block;

typedef struct cache {
    int size;
    int block_size;
    int associativity;
    int replacement_policy;
    cache_block* blocks;
    //  Add more fields for your cache, e.g., replacement policy data, statistics, etc.
} cache;

//  Function to simulate cache access
    //  Implement the cache lookup and miss handling logic
    //  Return 1 if cache hit, 0 if cache miss   
int cache_access(cache* cache, unsigned int address, int length);

//  Function to print simulation results
void print_sim_results(char* trace_file, int cache_size, int block_size, int associativity, char* rr_name, int physical_memory);

//  Function to parse memory unit to KB
int parse_memory_size(char *mem_size);
#endif /* MAIN_H */