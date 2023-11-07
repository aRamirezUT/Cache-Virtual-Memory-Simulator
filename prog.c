#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h> // Include getopt header

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
    // Add more fields for your cache, e.g., replacement policy data, statistics, etc.
} cache;

// Function to simulate cache access
int cache_access(cache* cache, unsigned int address, int length) {
    // Implement the cache lookup and miss handling logic
    // Return 1 if cache hit, 0 if cache miss
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    int cache_size = 512;
    int block_size = 16;
    int associativity = 2;
    int replacement_policy = 0; // 0 for RR, 1 for RND
    char* trace_file = NULL;

    int c;
    while ((c = getopt(argc, argv, "f:s:b:a:r:")) != -1) {
        switch (c) {
            case 'f':
                trace_file = strdup(optarg);
                break;
            case 's':
                cache_size = atoi(optarg);
                break;
            case 'b':
                block_size = atoi(optarg);
                break;
            case 'a':
                associativity = atoi(optarg);
                break;
            case 'r':
                if (strcmp(optarg, "RR") == 0) {
                    replacement_policy = 0;
                } else if (strcmp(optarg, "RND") == 0) {
                    replacement_policy = 1;
                } else {
                    fprintf(stderr, "Invalid replacement policy: %s\n", optarg);
                    exit(1);
                }
                break;
            default:
                fprintf(stderr, "Usage: ./cache_simulator -f <trace file> -s <cache size> -b <block size> -a <associativity> -r <replacement policy>\n");
                exit(1);
        }
    }

    if (trace_file == NULL) {
        fprintf(stderr, "Missing trace file\n");
        exit(1);
    }

    // Initialize cache
    cache* cache = malloc(sizeof(cache));
    cache->size = cache_size;
    cache->block_size = block_size;
    cache->associativity = associativity;
    cache->replacement_policy = replacement_policy;
    cache->blocks = malloc((cache_size / block_size) * sizeof(cache_block));

    // Simulate cache accesses
    FILE* fp = fopen(trace_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open trace file: %s\n", trace_file);
        exit(1);
    }

    int total_accesses = 0;
    int cache_hits = 0;
    int cache_misses = 0;

    char line[1024];
    while (fgets(line, sizeof(line), fp) != NULL) {
        unsigned int address;
        int length;

        if (strncmp(line, "EIP", 3) == 0) {
            // Instruction fetch
            sscanf(line, "EIP (%d): %x", &length, &address);
        } else {
            // Data access
            sscanf(line, "dstM: %x srcM: %x", &address, &address);
            length = 4;
        }

        total_accesses++;

        int hit = cache_access(cache, address, length);
        if (hit) {
            cache_hits++;
        } else {
            cache_misses++;
        }
    }

    fclose(fp);

    free(cache->blocks);
    free(cache);
    free(trace_file);

    // Print simulation results
    printf("Cache Simulator CS 3853 Fall 2023\n");
    printf("MILESTONE #2: - Simulation Results\n");
    printf("***** CACHE SIMULATION RESULTS *****\n");
    printf("Total Cache Accesses: %d\n", total_accesses);
    printf("Cache Hits: %d\n", cache_hits);
    printf("Cache Misses: %d\n", cache_misses);

    double hit_rate = (double)cache_hits / total_accesses * 100.0;
    double miss_rate = 100.0 - hit_rate;
    printf("***** ***** CACHE HIT & MISS RATE: ***** *****\n");
    printf("Hit Rate: %.2f%%\n", hit_rate);
    printf("Miss Rate: %.2f%%\n", miss_rate);

    return 0;
}
