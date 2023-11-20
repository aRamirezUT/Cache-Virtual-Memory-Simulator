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
    int accessed; // 0 if its has not been accessed 1 if it has been accessed
    unsigned char *data;
} cache_block;

typedef struct cache {
    int size;
    int block_size;
    int associativity;
    int replacement_policy; //  0 for RR, 1 for RND
    cache_block* blocks;
} cache;

cache_block *current_block;

//  Function to simulate cache access
    //  Implement the cache lookup and miss handling logic
    //  Return 1 if cache hit, 0 if cache miss   
int cache_access(cache* cache, unsigned int address, int length, cache_block **block, int* compulsory_misses, int* conflict_misses);

//  Function to print simulation results
void print_sim_results(char* trace_file, int cache_size, int block_size, int associativity, char* rr_name, int physical_memory, int total_accesses, int cache_hits, int cache_misses, int compulsory_misses, int conflict_misses, int total_instructions, double total_cycles);

//  Function to parse memory unit to KB
int parse_memory_size(char *mem_size);
#endif /* MAIN_H */

cache* create_cache(int cache_size, int block_size, int associativity, int replacement_policy);