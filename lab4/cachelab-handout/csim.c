/* lab4 assignment A */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "cachelab.h"

#define MAX_FILE_NAME_LEN 256 /* trace文件名的最大长度 */
#define MAX_LINE_LEN 1000   /* 读取trace文件中每行的最大长度 */

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
    int set_bits_num = 0;    /* Number of set index bits */
    int lines_num = 0;       /* Number of lines per set. */
    int block_bits_num = 0;  /* Number of block offset bits*/
    char trace_file[MAX_FILE_NAME_LEN] = ""; /* Trace file name */

    char buf[MAX_LINE_LEN]; /* 存储trace文件中每行的内容 */
    long unsigned addr;     /* trace文件每行中的地址 */
    int set_bit_index;      /* addr中set index bit的子位掩码 */  
    char c;

    int hits = 0;      /* number of  hits */
	int misses = 0;    /* number of misses */
	int evictions = 0; /* number of evictions */

    while((c=getopt(argc,argv,"vs:E:b:t:")) != -1){
        switch(c){
        case 'v':
            verbose = 1;
            break;
        case 's':
            set_bits_num = atoi(optarg);
            break;
        case 'E':
            lines_num = atoi(optarg);
            break;
        case 'b':
            block_bits_num = atoi(optarg);
            break;
        case 't':
            strncpy(trace_file, optarg, MAX_FILE_NAME_LEN);
            break;
        case '?':
        default:
            usage(argv);
            exit(1);
        }
    }

    if (set_bits_num <= 0 || lines_num <= 0 || block_bits_num <= 0 || trace_file[0] == '\0') {
        printf("Error: Missing required argument\n");
        usage(argv);
        exit(1);
    }

    /* set为cache中的一组，因为不需要具体存放的block，简化了set结构 */
    typedef struct
    {   
        long tag[lines_num];
        int vaild[lines_num]; /* cache中每组 */
        int lru[lines_num];   /*用于evict过程中判断替换哪一行*/
    } set;
    
    /* 申请内存模拟cache并初始化vaild */
    set* cache_ptr = malloc((1 << set_bits_num)* sizeof(set));
    if(cache_ptr == 0) {
        exit(1);
    }

    for (int i = 0; i < (1 << set_bits_num); i++)
        for (int j = 0; j < lines_num; j++)
            cache_ptr[i].vaild[j] = 0;

    FILE* fp; 
    fp = fopen(trace_file, "r"); 
    if (fp == NULL) {
        printf("Trace file %s doesn't exist.", trace_file);
    }
    
    /* 根据set_bits_num 和 block_bits_num 计算 set_bit_index */
    int s = set_bits_num - 1;
    while (s >= 0) {  
        set_bit_index |= 1 << (s + block_bits_num);  
        s -= 1; 
    }

    while (fgets(buf, MAX_LINE_LEN, fp) != NULL) {
        int i;
        sscanf(buf, " %c %lx,%d", &c, &addr, &i);

        int g = (set_bit_index & addr) >> block_bits_num;
        long unsigned tag = addr >> (set_bit_index + block_bits_num);

        printf("%lx, %d\n", tag, g);     

    }
    if (verbose) {
        printf("verbose\n");
    }
    fclose(fp);
    free(cache_ptr);

    printSummary(hits, misses, evictions);
    return 0;
}
