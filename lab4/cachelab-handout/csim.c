/* lab4 assignment A */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "cachelab.h"

/*
 * usage - Print usage info
 */
void usage(char *argv[]){
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h          Print this help message.\n");
    printf("  -v          Optional verbose flag.\n");
    printf("  -s <num>    Number of set index bits.\n");
    printf("  -E <num>    Number of lines per set.\n");
    printf("  -b <num>    Number of block offset bits.\n");
    printf("  -t <file>   Trace file.\n");
    printf("\n");
    printf("Example:\n"); 
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);      
}


int main(int argc, char* argv[])
{
    int verbose = 0;
    int s = 0;
    int E = 0;
    int b = 0;
    int groupNum = 0;
    char traceFile[100];
    char c;

    traceFile[0] = '\0';

    while((c=getopt(argc,argv,"vs:E:b:t:")) != -1){
        switch(c){
        case 'v':
            verbose = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            strncpy(traceFile, optarg, 99);
            break;
        case '?':
        default:
            usage(argv);
            exit(1);
        }
    }

    if (s <= 0 || E <= 0 || b <= 0 || traceFile[0] == '\0') {
        printf("Error: Missing required argument\n");
        usage(argv);
        exit(1);
    }

    groupNum = 1 << s;

    /* define the struct of a line, according to Programming Rules for Part A, memory accesses 
     * are aligned properly, we don't need define cache block. */
    typedef struct 
    {   
        long tag; 
        int vaild;
    }  line;

    /* define the struct of a group */
    typedef struct csim
    {
        line lines[E];
    } group;
    
    /* allocate the memory to simulate cache */
    group* cachePtr = (group*) malloc(groupNum * sizeof(group));
    if(cachePtr == 0) {
        exit(1);
    }

    /* init the vaild bit*/
    for (int i = 0; i < groupNum; i++)
        for (int j = 0; j < E; j++)
            cachePtr[i].lines[j].vaild = 0;

    

    free(cachePtr);
    printf("%d %d %d %d %s\n", verbose, s, E, b, traceFile);

    printSummary(0, 0, 0);
    return 0;
}
