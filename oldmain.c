#include "main.h"

int main(int argc, char* argv[]) {
    //  Parse command-line arguments and initialize variables
    int cache_size = 0;
    int block_size = 0;
    int associativity = 0;
    int replacement_policy = 0; //  0 for RR, 1 for RND
    int total_accesses = 0;
    int cache_hits = 0;
    int cache_misses = 0;
    int physical_memory = 0;
    int compulsory_misses = 0;
    int conflict_misses = 0;
    int total_instructions = 0;
    int i = 0;

    double total_cycles = 0.0;
    
    char* trace_files[3] = {NULL, NULL, NULL};
    char rr_name[12];
    // FILE* fp[3];
    
    int c; //   Parse command line arguments
    while ((c = getopt(argc, argv, "f:s:b:a:r:p:")) != -1) {
        switch (c) {
            case 'f':
                if (!trace_files[0]) {
                    trace_files[0] = strdup(optarg);
                } else if (!trace_files[1]) {
                    trace_files[1] = strdup(optarg);
                } else if (!trace_files[2]) {
                    trace_files[2] = strdup(optarg);
                } else {
                    fprintf(stderr, "Error: Maximum of 3 trace files allowed.\n");
                    exit(1);
                }
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
                    strcpy(rr_name, "Round Robin");
                } else if (strcmp(optarg, "RND") == 0) {
                    replacement_policy = 1;
                    strcpy(rr_name, "Random");
                } else {
                    fprintf(stderr, "Invalid replacement policy: %s\n", optarg);
                    exit(1);
                }
                break;
            case 'p':
                physical_memory = parse_memory_size(optarg);
                break;
            default:
                fprintf(stderr, "Usage: ./cache_simulator -f <trace file> -s <cache size> -b <block size> -a <associativity> -r <replacement policy> -p <physical memory in KB>\n");
                exit(1);
        }
    }
    //  Initialize cache
    cache* cache = create_cache(cache_size, block_size, associativity, replacement_policy);
    cache_block *block;

    //  Open trace file(s)
    for (i = 0; i < 3; i++){
        if(trace_files[i]){
            FILE* fp = fopen(trace_files[i], "r");
            if(fp == NULL){
                fprintf(stderr, "Failed to open trace file: %s\n", trace_files[i]);
                exit(1);
            }

            if (cache == NULL){
                //Handle cache creation error
                fprintf(stderr, "Failed to create cache!\n");
                exit(1);
            }

            char line[1024]; // Process trace file(s)
            while (fgets(line, sizeof(line), fp) != NULL) {
                unsigned int address;
                int length;

                if (strncmp(line, "EIP", 3) == 0) {
                    //  Instruction fetch
                    sscanf(line, "EIP (%d): %x", &length, &address);
                    total_instructions++; //Increment for each instruction fetch
                    total_cycles += 4; // 4 clock cycles for accessing main memory
                } else {
                    //  Data access
                    sscanf(line, "dstM: %x srcM: %x", &address, &address);
                    length = 4;
                    total_instructions++; //Increment for each data access

                    int hit = cache_access(cache, address, length, &block);
                    if (hit) {
                        cache_hits++;
                        total_cycles += 1; // 1 clock cycle for cache hit
                        printf("Cache HIT increment total_cycles by 1\n");
                    } else {
                        printf("Cache MISS, increment cache_misses by 1\n");
                        cache_misses++;
                        if (!block->accessed){
                            printf("compulsory_misses increment by 1\n");
                            compulsory_misses++;
                        } else{
                            printf("conflict_misses increment by 1\n");
                            conflict_misses++;
                        }
                        printf("total_cycles increment by 4 aka cache miss\n");
                        total_cycles += 4; // 4 clock cycles for cache miss
                    }
                }
                total_accesses++;
            }// End of processing trace file(s)

            fclose(fp);
            print_sim_results(trace_files[i], cache_size, block_size, associativity, rr_name, physical_memory, total_accesses, cache_hits, cache_misses, compulsory_misses, conflict_misses, total_instructions, total_cycles);
        } else {
            break;
        }
    }

    free(cache->blocks);
    free(cache);

    return 0;
}

/* --------------   START OF FUNCTION DECLARATIONS  -------------- */

int parse_memory_size(char *mem_size) {
    int size;
    char unit[3];

    // Parse the size and unit from the argument
    sscanf(mem_size, "%d%s", &size, unit);

    // Convert the size to kilobytes
    if (strcmp(unit, "GB") == 0) {
        size *= 1024 * 1024; // Convert gigabytes to kilobytes
    } else if (strcmp(unit, "MB") == 0) {
        size *= 1024; // Convert megabytes to kilobytes
    } else if (strcmp(unit, "KB") != 0) {
        // Invalid unit
        fprintf(stderr, "Error: Invalid unit for physical memory size.\n");
        exit(1);
    }

    return size;
}

/*  'compulsory_misses' and 'conflict_misses' are both calculated in the main() after calling cache_access  
*
*   Return 1 if cache hit, 0 if cache miss   
*/
int cache_access(cache* cache, unsigned int address, int length, cache_block** block) {
    // Assume direct-mapped cache for now
    int block_size = cache->block_size;
    int index_bits = log2(cache->size / block_size);
    int offset = address & (block_size - 1);
    int index = (address >> (int)log2(block_size)) & ((1 << index_bits) - 1);
    unsigned int tag = address >> (index_bits + (int)log2(block_size));

    *block = &cache->blocks[index];

    if ((*block)->valid && (*block)->tag == tag) {
        return 1; // Cache hit
    } else {
        // Cache miss - update the block with new tag and mark as valid
        printf("Cache miss - entering if block\n");
        if (!(*block)->accessed) {
            printf("entered cache miss if-block brackets\n");
            (*block)->accessed = 1; // Mark the block as accessed
            // Update your compulsory miss count here or in the calling function
            // Compulsory miss aand others are calculated in the calling function
        }

        (*block)->valid = 1;
        (*block)->tag = tag;
        // Load data into the cache block (simulation purposes)
        // ...

        return 0; // Cache miss
    }
}


// Inside your create_cache function
cache* create_cache(int cache_size, int block_size, int associativity, int replacement_policy) {
    cache* new_cache = malloc(sizeof(cache));
    if (new_cache == NULL) {
        // Handle allocation error
        return NULL;
    }
    
    new_cache->size = cache_size;
    new_cache->block_size = block_size;
    new_cache->associativity = associativity;
    new_cache->replacement_policy = replacement_policy;

    // Allocate memory for cache blocks
    new_cache->blocks = malloc((cache_size / block_size) * sizeof(cache_block));
    if (new_cache->blocks == NULL) {
        // Handle allocation error
        free(new_cache);
        return NULL;
    }

    // Initialize each cache block
    for (int i = 0; i < (cache_size / block_size); i++) {
        new_cache->blocks[i].valid = 0;
        new_cache->blocks[i].tag = 0;
        new_cache->blocks[i].accessed = 0;
        // Allocate memory for each block's data
        new_cache->blocks[i].data = malloc(block_size * sizeof(unsigned char));
        if (new_cache->blocks[i].data == NULL) {
            // Handle allocation error
            // Free previously allocated memory and return NULL
            for (int j = 0; j < i; j++) {
                free(new_cache->blocks[j].data);
            }
            free(new_cache->blocks);
            free(new_cache);
            return NULL;
        }
    }

    return new_cache;
}



void print_sim_results(char* trace_file, int cache_size, int block_size, int associativity, char* rr_name, int phyiscal_memory, int total_accesses, int cache_hits, int cache_misses, int compulsory_misses, int conflict_misses, int total_instructions, double total_cycles){
    printf("Cache Simulator - CS 3853 Fall 2023\n\n");
    printf("Trace File: %s\n\n", trace_file);
    printf("MILESTONE #1: - Simulation Results\n\n");
    printf("***** Cache Input Parameters *****\n");
    printf("Cache Size: %d KB\n", cache_size);
    printf("Block Size: %d bytes\n", block_size);
    printf("Associativity: %d\n", associativity);
    printf("Replacement Policy: %s\n", rr_name);

    int num_blocks = (cache_size * 1024) / block_size;
    int num_rows = num_blocks / associativity;
    int index_size = (int)log2(num_rows);
    int tag_size = 32 - index_size - (int)log2(block_size);
    int overhead_size = ((tag_size + 1) * associativity * num_rows) / 8;
    float implementation_memory_size = (num_blocks * block_size + overhead_size) / 1024.0;
    float cost = (implementation_memory_size) * 0.09;

    printf("\n***** Cache Calculated Values *****\n\n");
    printf("Total # Blocks: %d\n", num_blocks);
    printf("Tag Size: %d bits\n", tag_size);
    printf("Index Size: %d bits\n", index_size);
    printf("Total # Rows: %d\n", num_rows);
    printf("Overhead Size: %d bytes\n", overhead_size);
    printf("Implementation Memory Size: %.2f KB (%d bytes)\n", implementation_memory_size, (int)implementation_memory_size*1024);
    printf("Cost: $%.2f @ ($0.09 / KB)\n\n", cost);

    printf("MILESTONE #2: - Simulation Results\n");
    printf("***** CACHE SIMULATION RESULTS *****\n");
    printf("Total Cache Accesses: %d\n", total_accesses);
    printf("Cache Hits: %d\n", cache_hits);
    printf("Cache Misses: %d\n", cache_misses);
    printf("--- Compulsory Misses: %d\n", compulsory_misses);
    printf("--- Conflict Misses: %d\n", conflict_misses);

    double hit_rate = (double)cache_hits / total_accesses * 100.0;
    double miss_rate = 100.0 - hit_rate;
    printf("***** ***** CACHE HIT & MISS RATE: ***** *****\n");
    printf("Hit Rate: %.2f%%\n", hit_rate);
    printf("Miss Rate: %.2f%%\n", miss_rate);

    double cpi = total_cycles / total_instructions;

    double unused_kb = ((num_blocks - compulsory_misses) * (block_size + overhead_size)) / 1024.0;
    double unused_percentage = (unused_kb / (cache_size * 1024)) * 100.0;
    double waste = cost / 1024.0 * unused_kb;

    printf("CPI: %.2f Cycles/Instruction\n", cpi);
    printf("Unused Cache Space: %.2f KB / %.2d KB = %.2f%% Waste: $%.2f\n", unused_kb, cache_size, unused_percentage, waste);
}

    /* SIDE NOTES
        We will want to redo the calculations as needed for Milestone #1 with the proper pointers to the cache.
        but for now this works
    */