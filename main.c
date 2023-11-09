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
    int printed_addresses = 0;
    int physical_memory = 0;
    int i = 0;
    char* trace_files[3] = {NULL, NULL, NULL};
    char rr_name[4];
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
    cache* cache = malloc(sizeof(cache));
    cache->size = cache_size;
    cache->block_size = block_size;
    cache->associativity = associativity;
    cache->replacement_policy = replacement_policy;
    cache->blocks = malloc((cache_size / block_size) * sizeof(cache_block));

    //  Open trace file(s)
    for (i = 0; i < 3; i++){
        if(trace_files[i]){
            FILE* fp = fopen(trace_files[i], "r");
            if(fp == NULL){
                fprintf(stderr, "Failed to open trace file: %s\n", trace_files[i]);
                exit(1);
            }

            char line[1024]; // Process trace file(s)
            while (fgets(line, sizeof(line), fp) != NULL) {
                unsigned int address;
                int length;

                if (strncmp(line, "EIP", 3) == 0) {
                    //  Instruction fetch
                    sscanf(line, "EIP (%d): %x", &length, &address);
                } else {
                    //  Data access
                    sscanf(line, "dstM: %x srcM: %x", &address, &address);
                    length = 4;
                }

                total_accesses++;

                //  Print the first none zero 20 addresses and lengths
                if(address != 0){
                    if (printed_addresses < 20){
                        printf("0x%x: (%d)\n", address, length);
                        printed_addresses++;
                    }
                }

                int hit = cache_access(cache, address, length);
                if (hit) {
                    cache_hits++;
                } else {
                    cache_misses++;
                }
            } // End of processing trace file(s)
            fclose(fp);
            print_sim_results(trace_files[i], cache_size, block_size, associativity, rr_name, physical_memory);
        } else {
            break;
        }
    }

    free(cache->blocks);
    free(cache);

    return 0;
}

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

int cache_access(cache* cache, unsigned int address, int length){
    //  Implement the cache lookup and miss handling logic
    //  Return 1 if cache hit, 0 if cache miss   
    return 0;
}

void print_sim_results(char* trace_file, int cache_size, int block_size, int associativity, char* rr_name, int phyiscal_memory){
    printf("Cache Simulator - CS 3853 Fall 2023\n\n");
    printf("Trace File: %s\n\n", trace_file);
    printf("MILESTONE #1: - Simulation Results\n\n");
    printf("***** Cache Input Parameters *****\n");
    printf("Cache Size: %d KB\n", cache_size);
    printf("Block Size: %d bytes\n", block_size);
    printf("Associativity: %d\n", associativity);
    printf("Replacement Policy: %s\n", rr_name);

    //  Calculations as needed for Milestone #1
    //int tag_size = 32 - (int)(log2((double)num_blocks) + log2((double)block_size));
    //int index_size = (int)log2(num_blocks / associativity);
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
}

    /* SIDE NOTES
        We will want to redo the calculations as needed for Milestone #1 with the proper pointers to the cache.
        but for now this works

      MILESTONE #2
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