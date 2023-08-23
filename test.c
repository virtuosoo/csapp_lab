#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

static int A[1030][1030];
static int B[1030][1030];

void simple_trans(int N, int M, int A[N][M], int B[N][M])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            B[j][i] = A[i][j];
        }
    }  
}

void init(int N, int M, int A[N][M])
{
    int t = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            A[i][j] = ++t;
        }
    }
}

int main(int argc, char *argv[]) {
    int S = 1025;
    init(S, S, A);
    clock_t startTime = clock();
    simple_trans(S, S, A, B);
    clock_t endTIme = clock();
    printf("S:%d, spend(%ld)\n",S, endTIme - startTime);
    return 0;
}