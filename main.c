#include "main.h"

void handleIncorrectUsage(char* errorMessage) {

    printf("\n%s Please launch using:\n", errorMessage);
    printf("./sim -f Trace1.trc -s 512 -b 16 -a 8 -r RND\n\n");

    printf("-f <trace file name> [ name of text file with the trace ]\n");
    printf("-s <cache size in KB> [ 1 KB to 8 MB ]\n");
    printf("-b <block size> [ 4 bytes to 64 bytes ]\n");
    printf("-a <associativity> [ 1, 2, 4, 8, 16 ]\n");
    printf("-r <replacement policy> [ RR or RND]\n\n");
}

void doCacheMath(varStruct* vars, argStruct* args) {
    vars->total_blocks = (args->cache_size*1024) / args->block_size;
    vars->index_size = round(log( (args->cache_size*1024) / (args->associativity * args->block_size) )/log(2)); //bits
    vars->offset_size = round(log(args->block_size)/log(2)); //bits
    vars->tag_size = 32 - vars->offset_size - vars->index_size; //bits
    vars->total_indices = pow(2, vars->index_size);
    vars->overhead_memory_size = ( (1+vars->tag_size)*args->associativity * vars->total_indices ) / 8; //in bytes
    vars->implementation_memory_size = ((vars->total_blocks * args->block_size) + vars->overhead_memory_size) / 1024; //in KB

    vars->cache_hit_rate = -1; //percent
    vars->cpi = -1; //cycles per instruction
}
    
void showCalculatedValues(varStruct* vars) {
    printf("***** Cache Calculated Value *****\n\n");
    printf("Total # Bocks: %d\n", vars->total_blocks);
    printf("Tag Size: %d bits\n", vars->tag_size);
    printf("Index Size: %d bits\n", vars->index_size);
    printf("Total # Rows: %d\n", vars->total_indices);
    printf("Overhead Size: %d bytes\n", vars->overhead_memory_size);
    printf("Implementation Memory Size: %d KB (%d bytes)\n",
        vars->implementation_memory_size, vars->implementation_memory_size * 1024);
    printf("Cost: $%.2f @ ($0.09 / KB)\n\n",
        (vars->implementation_memory_size) * 0.09);
    
}

int main(int argc, char* argv[]) {
    //check for correct usage
    int maxArgs = 5;
    if((argc % 2 == 0 || argc < 3 || argc > 1 + (maxArgs * 2)) && argc != 1) {
        handleIncorrectUsage("Invalid number of arguments.");
        exit(-1);
    }

    //initialize argument variables
    argStruct* args = (argStruct*) malloc(sizeof(argStruct));
    args->trace_file_name = NULL;
    args->cache_size = 0; //in KB
    args->block_size = 0; //in bytes
    args->associativity = 0;
    args->replacement_policy = NULL; //will be RR or RND

    //read in arguments
    int i;
    for(i=1; i < argc; i+=2 ) {
        if(strcmp(argv[i], "-f") == 0) {
            args->trace_file_name = argv[i+1];
        } else if(strcmp(argv[i], "-s") == 0) {
            args->cache_size = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-b") == 0) {
            args->block_size = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-a") == 0) {
            args->associativity = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-r") == 0) {
            args->replacement_policy = argv[i+1];
        }
    }

    varStruct* vars = (varStruct*) malloc(sizeof(varStruct)); //declared here so it can be free'd after use
    if(args->trace_file_name == NULL) {
        handleIncorrectUsage("Trace file not included.");
        free(args);
        free(vars);
        exit(-1);
    }

    //read in file
    Queue* fileContents = readFile(args->trace_file_name); //queue consists of (char*) void* to each line

    //process file
    Queue* traceData = convertData(fileContents);

    doCacheMath(vars, args);

    //run simulation
    resultDataStruct* resDt = runSimulation(traceData, args, vars);

    float percentCacheMat = (((vars->implementation_memory_size)-(resDt->usedCacheSpace / 1024)) / vars->implementation_memory_size) * 100;
    float unused_cache_space = (vars->implementation_memory_size)-(resDt->usedCacheSpace / 1024);


    //print output report
    printf("Cache Simulator CS 3853 Fall 2023 - Group #20\n\nMILESTONE #1: Input Parameters and Calculated Values\n\n");
    printf("Trace File: %s\n\n", args->trace_file_name);
    printf("***** Cache Input Parameters *****\n");
    printf("Cache Size: %d KB\n", args->cache_size);
    printf("Block Size: %d bytes\n", args->block_size);
    printf("Associativity: %d\n", args->associativity);
    printf("Replacement Policy: %s\n\n", args->replacement_policy);

    showCalculatedValues(vars);

    printf("\nMILESTONE #2 - SIMULATION RESULTS\n\n***** CACHE SIMULATION RESULTS *****\n\n");
    printf("Total Cache Accesses:   %d   (%d addresses)\n", resDt->totalCacheAccesses, resDt->numBlocksAccessed);
    printf("Cache Hits:             %d\n", resDt->cacheHits);
    printf("Cache Misses:           %d\n", resDt->totalCacheAccesses - resDt->cacheHits);
    printf("--- Compulsory Misses:  %d\n", resDt->compulsoryMisses);
    printf("--- Conflict Misses:    %d\n\n", resDt->conflictMisses);

    printf("***** ***** CACHE HIT & MISS RATE ***** *****\n\n");
    printf("Cache Hit Rate: %.4f%%\n", vars->cache_hit_rate);
    printf("Cache Miss Rate: %.4f%%\n", 100 - vars->cache_hit_rate);
    printf("CPI: %.2f cycles/instruction\n\n", vars->cpi);
    printf("Unused Cache Space: %.2f KB / %.2d KB = %.2f%%  Waste: $%.2f\n",
        unused_cache_space, vars->implementation_memory_size, percentCacheMat, (unused_cache_space * 0.09));
    printf("Unused Cache Blocks: %d / %d\n\n", (vars->total_blocks)-(resDt->usedCacheBlocks), vars->total_blocks);    

    free(resDt->numBlkAcsCntArry);
    free(resDt);
    free(args);
    free(vars);

    // printf("PROGRAM COMPLETE\n\n");
    return 0;
}

/* Function definitions */

bool numberIsPower(int number, int power) {
    return lowerPrecision(round(log(number)/log(power)), 2) == lowerPrecision(log(number)/log(power), 2);
}

double lowerPrecision(double number, int precision) {
    int factor = pow(10, precision);
    return floor(number*factor)/factor;
}
