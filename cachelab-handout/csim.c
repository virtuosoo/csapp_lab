#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>

const char *optString = "v::s:E:b:t:";
const int MAX_TRACE_SIZE = 128;
bool printDetail = false;
FILE *traceFile;
int s, E, b;
int hits = 0, misses = 0, evictions = 0;
long long setIdxMask, tagMask; 

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
bool isTagInSet(cacheSet *s, long long tag);

void read_params(int argc, char *argv[]);
void process();

//返回掩码， 从低位起的第s位到第e位都是1
long long getMask(int s, int e);

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
    setIdxMask = getMask(b, b + s - 1);
    tagMask = getMask(b + s, 63);
}

void process() 
{
    int setSize = (1<<s);
    cacheSet *sets = malloc(setSize * sizeof(cacheSet));
    for (int i = 0; i < setSize; ++i) {
        initCacheSet(&sets[i], i);
    }
    char traceType[MAX_TRACE_SIZE];
    int addr, size;
    while (1) {
        if (fscanf(traceFile, "%s", traceType) == EOF) {
            break;
        }
        fscanf(traceFile, "%x,%x", &addr, &size);
        long long idx = (addr & setIdxMask);
        cacheSet *set = &sets[idx];
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

bool isTagInSet(cacheSet *s, long long tag)
{
    LinelistNode *cur = s->head->next;
    while (cur != s->tail) {
        if (cur->line->tag == tag) {
            return true;
        }
    }
    return false;
}

bool insertToSet(cacheSet *s, int tag)
{
    LinelistNode *a = malloc(sizeof(LinelistNode));
    a->line = malloc(sizeof(Line_t));
    a->line->tag = tag;
    addToList(a, s->head->next);
    s->listSize += 1;
}

bool isSetFull(cacheSet *s)
{
    return s->listSize >= E;
}

void evictFromSet(cacheSet *s)
{
    removeFromList(s->tail->prev);
    s->listSize -= 1;
}


long long getMask(int s, int e)
{
    long long mask = 0;
    for (int i = s; i <= e; ++i) {
        mask |= (1l << i);
    }
    return mask;
}