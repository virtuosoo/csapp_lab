#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>

const char *optString = "v::s:E:b:t:";
const int MAX_TRACE_SIZE = 256;
bool printDetail = false;
FILE *traceFile;
int s, E, b;
int hits = 0, misses = 0, evictions = 0;

typedef struct Line {
    int tag;         // 标记位
    int timestamp;
} Line_t;

typedef struct LinelistNode{
    struct LinelistNode *next, *prev;
    Line_t *line;
} LinelistNode;

// 从链表中删去o
void removeFromList(LinelistNode *o);
//将节点a插入到链表中的b节点之前
void addToList(LinelistNode *a, LinelistNode *b);

typedef struct cacheSet {
    LinelistNode *head, *tail;
    int listSize;
    int idx;
} cacheSet;

void initCacheSet(cacheSet *c, int idx);

void read_params(int argc, char *argv[]);
void process();

int main(int argc, char *argv[])
{
    read_params(argc, argv);
    process();
    return 0;
}

void read_params(int argc, char *argv[])
{
    int o;
    while ((o = getopt(argc, argv, optString)) != -1) {
        switch (o) {
            case 'v':
                printDetail = true;
                break;
            case 's':
                s = atoi(optarg);
                if (s == 0) {
                    printf("unvalid param for s");
                }
                break;
            case 'E':
                E = atoi(optarg);
                if (E == 0) {
                    printf("unvalid param for E");
                }
                break;
            case 'b':
                b = atoi(optarg);
                if (b == 0) {
                    printf("unvalid param for b");
                }
                break;
            case 't':
                traceFile = fopen(optarg, "r");
                if (traceFile == NULL) {
                    printf("open trace file failed\n");
                }
                break;
            default:
                printf("unknown para\n");
                break;
        }
    }
}

void process() 
{
    int setSize = (1<<s);
    cacheSet *sets = malloc(setSize * sizeof(cacheSet));
    for (int i = 0; i < setSize; ++i) {
        initCacheSet(&sets[i], i);
    }
    char traceRecoed[MAX_TRACE_SIZE];
    while (fscanf(traceFile, "%s", traceRecoed) != EOF) {
        printf("%s\n", traceRecoed);
    }
}

void removeFromList(LinelistNode *o)
{
    o->prev->next = o->next;
    o->next->prev = o->prev;
}

void addToList(LinelistNode *a, LinelistNode *b)
{
    LinelistNode *prev = b->prev;
    prev->next = a;
    b->prev = a;
    a->next = b;
    a->prev = prev;
}

void initCacheSet(cacheSet *c, int idx)
{
    c->head = malloc(sizeof(LinelistNode));
    c->tail = malloc(sizeof(LinelistNode));
    c->head->next = c->tail;
    c->tail->prev = c->head;

    c->head->prev = NULL;
    c->tail->next = NULL;
    c->listSize = 0;
    c->idx = idx;
}