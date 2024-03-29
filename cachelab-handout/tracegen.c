/* 
 * tracegen.c - Running the binary tracegen with valgrind produces
 * a memory trace of all of the registered transpose functions. 
 * 
 * The beginning and end of each registered transpose function's trace
 * is indicated by reading from "marker" addresses. These two marker
 * addresses are recorded in file for later use.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include "cachelab.h"
#include <string.h>

/* External variables declared in cachelab.c */
extern trans_func_t func_list[MAX_TRANS_FUNCS];
extern int func_counter; 

/* External function from trans.c */
extern void registerFunctions();

/* Markers used to bound trace regions of interest */
volatile char MARKER_START, MARKER_END;

static int A[256][256];
static int B[256][256];
static int M;
static int N;


int validate(int fn,int M, int N, int A[N][M], int B[M][N]) {
    int C[M][N];
    memset(C,0,sizeof(C));
    correctTrans(M,N,A,C);
    for(int i=0;i<M;i++) {
        for(int j=0;j<N;j++) {
            if(B[i][j]!=C[i][j]) {
                printf("Validation failed on function %d! Expected %d but got %d at B[%d][%d]\n",fn,C[i][j],B[i][j],i,j);
                return 0;
            }
        }
    }
    return 1;
}

void findConflict(int M, int N, int A[N][M], int B[M][N])
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            unsigned long long pa = (unsigned long long) &A[i][j], pb = (unsigned long long) &B[i][j];
            unsigned long long Aidx = (pa >> 5U) % 32U, Bidx = (pb >> 5U) % 32U;
            printf("i(%d), j(%d), Aidx(%lld), Bidx(%lld)\n",i, j, Aidx, Bidx);
        }
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            unsigned long long pa = (unsigned long long) &A[i][j], pb = (unsigned long long) &B[j][i];
            unsigned long long Aidx = (pa >> 5U) % 32U, Bidx = (pb >> 5U) % 32U;
            if (Aidx == Bidx) {
                printf("conflict: i(%d), j(%d), Aidx(%lld), Bidx(%lld)\n",i, j, Aidx, Bidx);
            }
        }
    }
}


void initEasy(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;
    int t = 0;
    for (i = 0; i < N; i++){
        for (j = 0; j < M; j++){
            A[i][j] = ++t;
        }
    }
}

void showMatrix(int N, int M, int A[N][M])
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[]){
    int i;

    char c;
    int selectedFunc=-1;
    while( (c=getopt(argc,argv,"M:N:F:")) != -1){
        switch(c){
        case 'M':
            M = atoi(optarg);
            break;
        case 'N':
            N = atoi(optarg);
            break;
        case 'F':
            selectedFunc = atoi(optarg);
            break;
        case '?':
        default:
            printf("./tracegen failed to parse its options.\n");
            exit(1);
        }
    }
  

    /*  Register transpose functions */
    registerFunctions();

    /* Fill A with data */
    initMatrix(M,N, A, B); 
    //initEasy(M, N, A, B);
    //showMatrix(N, M, A);
        //分析地址冲突情况
    findConflict(M, N, A, B);
    /* Record marker addresses */
    FILE* marker_fp = fopen(".marker","w");
    assert(marker_fp);
    fprintf(marker_fp, "%llx %llx", 
            (unsigned long long int) &MARKER_START,
            (unsigned long long int) &MARKER_END );
    fclose(marker_fp);

    if (-1==selectedFunc) {
        /* Invoke registered transpose functions */
        for (i=0; i < func_counter; i++) {
            MARKER_START = 33;
            (*func_list[i].func_ptr)(M, N, A, B);
            MARKER_END = 34;
            if (!validate(i,M,N,A,B))
                return i+1;
        }
    } else {
        MARKER_START = 33;
        (*func_list[selectedFunc].func_ptr)(M, N, A, B);
        MARKER_END = 34;
        //showMatrix(M, N, B);
        if (!validate(selectedFunc,M,N,A,B))
            return selectedFunc+1;

    }
    return 0;
}


