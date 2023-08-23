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
void showMatrix1(int N, int M, int A[N][M])
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
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
    } else if (M == 64 && N == 64) {
        for (int i = 0; i < 64; i += 8) { 
            for (int j = 0; j < 64; j += 8) {
                for (int k = i; k < i + 4; ++k) { //把a的前四行赋给b的前四行
                    t0 = A[k][j + 0];
                    t1 = A[k][j + 1];
                    t2 = A[k][j + 2];
                    t3 = A[k][j + 3];
                    t4 = A[k][j + 4];
                    t5 = A[k][j + 5];
                    t6 = A[k][j + 6];
                    t7 = A[k][j + 7];

                    B[j + 0][k] = t0;
                    B[j + 1][k] = t1;
                    B[j + 2][k] = t2;
                    B[j + 3][k] = t3;
                    B[j + 0][k + 4] = t4;
                    B[j + 1][k + 4] = t5;
                    B[j + 2][k + 4] = t6;
                    B[j + 3][k + 4] = t7;
                }

                for (int k = j; k < j + 4; ++k) {
                    //获取B的右上部分
                    t0 = B[k][i + 4];
                    t1 = B[k][i + 5];
                    t2 = B[k][i + 6];
                    t3 = B[k][i + 7];

                    //获取A的左下部分，这里一次性miss4次
                    t4 = A[i + 4][k];
                    t5 = A[i + 5][k];
                    t6 = A[i + 6][k];
                    t7 = A[i + 7][k];

                    //将A的左下写入B的右上
                    B[k][i + 4] = t4;
                    B[k][i + 5] = t5;
                    B[k][i + 6] = t6;
                    B[k][i + 7] = t7;

                    //将之前B右上部分写入左下部分， 这里每次迭代miss一次，总共4次
                    B[k + 4][i] = t0;
                    B[k + 4][i + 1] = t1;
                    B[k + 4][i + 2] = t2;
                    B[k + 4][i + 3] = t3;
                }

                for (int k = i + 4; k < i + 8; ++k) {
                    //此时A、B的右下都在缓存中
                    t0 = A[k][j + 4];
                    t1 = A[k][j + 5];
                    t2 = A[k][j + 6];
                    t3 = A[k][j + 7];

                    B[j + 4][k] = t0;
                    B[j + 5][k] = t1;
                    B[j + 6][k] = t2;
                    B[j + 7][k] = t3;
                }
            }
        }
    } else {
        int blockSize = 16, i = 0, j = 0;
        int ib = 0, jb = 0;
        for (i = 0; i < N; ) {
            ib = blockSize < (N - i) ? blockSize : (N - i);
            for (j = 0; j < M; ) {
                jb = blockSize < (M - j) ? blockSize : (M - j);
                for (int i1 = i; i1 < i + ib; ++i1) {
                    for (int j1 = j; j1 < j + jb; ++j1) {
                        B[j1][i1] = A[i1][j1];
                    }
                }
                j += jb;
            }
            i += ib;
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
    //int t0, t1, t2, t3, t4, t5, t6, t7;
    int blockSize = 16, i = 0, j = 0;
    int ib = 0, jb = 0;
    for (i = 0; i < N; ) {
        ib = blockSize < (N - i) ? blockSize : (N - i);
        for (j = 0; j < M; ) {
            jb = blockSize < (M - j) ? blockSize : (M - j);
            for (int i1 = i; i1 < i + ib; ++i1) {
                for (int j1 = j; j1 < j + jb; ++j1) {
                    B[j1][i1] = A[i1][j1];
                }
            }
            j += jb;
        }
        i += ib;
    }
}


char transpose_test64_desc[] = "Transpose test 64";
void transpose_test64(int M, int N, int A[N][M], int B[M][N])
{
    int t0, t1, t2, t3, t4, t5, t6, t7;
    for (int i = 0; i < N; i += 8) {
        for (int j = 0; j < M; j += 8) {
            for (int k = i; k < i + 4; ++k) { //把a的前四行赋给b的前四行
                t0 = A[k][j + 0];
                t1 = A[k][j + 1];
                t2 = A[k][j + 2];
                t3 = A[k][j + 3];
                t4 = A[k][j + 4];
                t5 = A[k][j + 5];
                t6 = A[k][j + 6];
                t7 = A[k][j + 7];

                B[j + 0][k] = t0;
                B[j + 1][k] = t1;
                B[j + 2][k] = t2;
                B[j + 3][k] = t3;
                B[j + 0][k + 4] = t4;
                B[j + 1][k + 4] = t5;
                B[j + 2][k + 4] = t6;
                B[j + 3][k + 4] = t7;
            }
            for (int k = i + 4; k < i + 8; ++k) { //后四行
                t0 = A[k][j + 0];
                t1 = A[k][j + 1];
                t2 = A[k][j + 2];
                t3 = A[k][j + 3];
                t4 = A[k][j + 4];
                t5 = A[k][j + 5];
                t6 = A[k][j + 6];
                t7 = A[k][j + 7];

                B[j + 4][k - 4] = t0;
                B[j + 5][k - 4] = t1;
                B[j + 6][k - 4] = t2;
                B[j + 7][k - 4] = t3;
                B[j + 4][k] = t4;
                B[j + 5][k] = t5;
                B[j + 6][k] = t6;
                B[j + 7][k] = t7;
            }

            for (int k = j; k < j + 4; ++k) {
                t0 = B[k][i + 4];
                t1 = B[k][i + 5];
                t2 = B[k][i + 6];
                t3 = B[k][i + 7];

                t4 = B[k + 4][i];
                t5 = B[k + 4][i + 1];
                t6 = B[k + 4][i + 2];
                t7 = B[k + 4][i + 3];

                B[k + 4][i] = t0;
                B[k + 4][i + 1] = t1;
                B[k + 4][i + 2] = t2;
                B[k + 4][i + 3] = t3;

                B[k][i + 4] = t4;
                B[k][i + 5] = t5;
                B[k][i + 6] = t6;
                B[k][i + 7] = t7;
            }
        }
    }
}

char transpose_64x64_ans_desc[] = "transpose_64x64_ans";
void transpose_64x64_ans(int M, int N, int A[N][M], int B[M][N])
{
    int a_0, a_1, a_2, a_3, a_4, a_5, a_6, a_7;
    for (int i = 0; i < 64; i += 8){
        for (int j = 0; j < 64; j += 8){
            for (int k = i; k < i + 4; k++){
                // 得到A的第1,2块
                a_0 = A[k][j + 0];
                a_1 = A[k][j + 1];
                a_2 = A[k][j + 2];
                a_3 = A[k][j + 3];
                a_4 = A[k][j + 4];
                a_5 = A[k][j + 5];
                a_6 = A[k][j + 6];
                a_7 = A[k][j + 7];
                // 复制给B的第1,2块
                B[j + 0][k] = a_0;
                B[j + 1][k] = a_1;
                B[j + 2][k] = a_2;
                B[j + 3][k] = a_3;
                B[j + 0][k + 4] = a_4;
                B[j + 1][k + 4] = a_5;
                B[j + 2][k + 4] = a_6;
                B[j + 3][k + 4] = a_7;
            }
            for (int k = j; k < j + 4; k++){
                // 得到B的第2块
                a_0 = B[k][i + 4];
                a_1 = B[k][i + 5];
                a_2 = B[k][i + 6];
                a_3 = B[k][i + 7];
                // 得到A的第3块
                a_4 = A[i + 4][k];
                a_5 = A[i + 5][k];
                a_6 = A[i + 6][k];
                a_7 = A[i + 7][k];
                // 复制给B的第2块
                B[k][i + 4] = a_4;
                B[k][i + 5] = a_5;
                B[k][i + 6] = a_6;
                B[k][i + 7] = a_7;
                // B原来的第2块移动到第3块
                B[k + 4][i + 0] = a_0;
                B[k + 4][i + 1] = a_1;
                B[k + 4][i + 2] = a_2;
                B[k + 4][i + 3] = a_3;
            }
            for (int k = i + 4; k < i + 8; k++)
            {
                // 处理第4块
                a_4 = A[k][j + 4];
                a_5 = A[k][j + 5];
                a_6 = A[k][j + 6];
                a_7 = A[k][j + 7];
                B[j + 4][k] = a_4;
                B[j + 5][k] = a_5;
                B[j + 6][k] = a_6;
                B[j + 7][k] = a_7;
            }
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
    registerTransFunction(transpose_test64, transpose_test64_desc);
    registerTransFunction(transpose_64x64_ans, transpose_64x64_ans_desc);

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

