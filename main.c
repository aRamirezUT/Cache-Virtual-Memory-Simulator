#include "main.h"

/*  Include Globar Variables here */
char *inputFile, *ALG;
int cpuBusy, ioBusy, totalTime, waitingTime, numProcs, turnAroundTime, quantum;


int main(int argc, char *argv[]) {

    /*  Check that required arguments are present */
    if (argc < 4) {
        fprintf(stderr, "Usage: %s prog -alg [FIFO|SJF|PR|RR] [-quantum [integer(ms)]] -input [file name]\n", argv[0]);
        exit(1);
    }
    /*  Perform a check if PR algorthm is read that -quantum input is also read */
    //  Implement here
    int i;
    for (i = 1; i < argc; i++) {    /*  Parse command line arguments */
        if (strcmp(argv[i], "-alg") == 0) {
            if (i + 1 < argc) {
                ALG = argv[i+1];
            }
        }
        else if (strcmp(argv[i], "-quantum") == 0) {
            if (i + 1 < argc) {
                quantum = atoi(argv[i+1]);
            }
        }
        else if (strcmp(argv[i], "-input") == 0) {
            if (i + 1 < argc) {
                inputFile = argv[i+1];
            }
        }
    }

    /*  Check to see if the cmd line arugments are read in correctly */
    printf("Algorithm: %s\n", ALG);
    if(quantum > 0 ) {
        printf("Quantum: %d\n", quantum);
    }
    printf("Input file: %s\n", inputFile);
}