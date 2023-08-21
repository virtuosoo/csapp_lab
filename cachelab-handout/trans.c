/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int t0, t1, t2, t3, t4, t5, t6, t7;
    if (M == 32 && N == 32) {
        int i = 0, j = 0, k1;
        for (i = 0; i < 32; i += 8) {
            for (j = 0; j < 32; j += 8) {
                for (k1 = i; k1 < i + 8; ++k1) {
                    t0 = A[k1][j]; t1 = A[k1][j + 1]; t2 = A[k1][j + 2]; t3 = A[k1][j + 3];
                    t4 = A[k1][j + 4]; t5 = A[k1][j + 5]; t6 = A[k1][j + 6]; t7 = A[k1][j + 7];

                    B[j][k1] = t0; B[j + 1][k1] = t1; B[j + 2][k1] = t2; B[j + 3][k1] = t3;
                    B[j + 4][k1] = t4; B[j + 5][k1] = t5; B[j + 6][k1] = t6; B[j + 7][k1] = t7;
                }
            }
        }
    }

    if (M == 64 && N == 64) {
        int blockSize = 4;
        int i = 0, j = 0, k1, k2;
        for (i = 0; i < N; i += blockSize) {
            for (j = 0; j < M; j += blockSize) {
                for (k1 = i; k1 < i + blockSize; ++k1) {
                    for (k2 = j; k2 < j + blockSize; ++k2) {
                        B[k2][k1] = A[k1][k2];
                    }
                }
            }
        }
    }
  
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

char transpose_test_desc[] = "Transpose test";
void transpose_test(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }   
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 
    registerTransFunction(transpose_test, transpose_test_desc);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

