#include "cache.h"

extern 
CacheNode *newEmptyNode()
{
    CacheNode *node = (CacheNode *) malloc(sizeof(CacheNode));
    node->url = node->data = node->next = node->prev = NULL;
    node->nodeSize = 0;
    return node;
}

void initCacheList(CacheList *list, int maxTotSize)
{
    list->totSize = 0;
    list->maxTotSize = maxTotSize;
    pthread_rwlock_init(&(list->rwlock), NULL);
    list->head = newEmptyNode();
    list->tail = newEmptyNode();
    list->head->next = list->tail;
    list->tail->prev = list->head;
}

static void removeFromList(CacheNode *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static moveToHead(CacheList *list, CacheNode *node)
{
    removeFromList(node);
    CacheNode *head = list->head;
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

CacheNode *findNodeByUrl(CacheList *list, char *url)
{
    pthread_rwlock_rdlock(&(list->rwlock));
    CacheNode *cur = list->head->next;
    while (cur != list->tail) {
        if (strcmp(cur->url, url) == 0) {
            break;
        }
    }

    if (cur == list->tail) {
        cur = NULL;
    }
    pthread_rwlock_unlock(&(list->rwlock));
    return cur;
}