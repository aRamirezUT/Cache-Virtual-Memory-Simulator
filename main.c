#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h> // Include getopt header
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
    // Add more fields for your cache, e.g., replacement policy data, statistics, etc.
} cache;

// Function to simulate cache access
int cache_access(cache* cache, unsigned int address, int length) {
    // Implement the cache lookup and miss handling logic
    // Return 1 if cache hit, 0 if cache miss   
    return 0;
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments and initialize variables
    int cache_size = 0;
    int block_size = 0;
    int associativity = 0;
    int replacement_policy = 0; // 0 for RR, 1 for RND
    char* trace_file = NULL;
    char rr_name[4];

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
                    strcpy(rr_name, "RR");
                } else if (strcmp(optarg, "RND") == 0) {
                    replacement_policy = 1;
                    strcpy(rr_name, "RND");
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

    // Open trace file
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
    printf("Cache Simulator CS 3853 Fall 2023\n\n");
    printf("MILESTONE #1: - Simulation Results\n\n");
    printf("***** Cache Input Parameters *****\n");
    printf("Cache Size: %d KB\n", cache_size);
    printf("Block Size: %d bytes\n", block_size);
    printf("Associativity: %d\n", associativity);
    printf("Replacement Policy: %s\n", rr_name);

    // Calculations as needed for Milestone #1
    int num_blocks = cache_size * 1024 / block_size;
    int tag_size = 32 - (int)(log2((double)num_blocks) + log2((double)block_size));
    int index_size = (int)log2(num_blocks / associativity);
    int num_rows = num_blocks / associativity;
    int overhead_size = num_blocks * (1 + sizeof(unsigned int) + block_size);
    float implementation_memory_size = overhead_size / 1024.0;
    float cost = implementation_memory_size * 0.09;

    printf("\n***** Cache Calculated Values *****\n");
    printf("Total # Blocks: %d\n", num_blocks);
    printf("Tag Size: %d bits\n", tag_size);
    printf("Index Size: %d bits\n", index_size);
    printf("Total # Rows: %d\n", num_rows);
    printf("Overhead Size: %d bytes\n", overhead_size);
    printf("Implementation Memory Size: %.2f KB (%d bytes)\n", implementation_memory_size, overhead_size);
    printf("Cost: $%.2f @ ($0.09 / KB)\n", cost);

    /* SIDE NOTES
        We will want to redo the calculations as needed for Milestone #1 with the proper pointers to the cache.
        but for now this works
    */
    /*  MILESTONE #2
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

    */
    return 0;
}
