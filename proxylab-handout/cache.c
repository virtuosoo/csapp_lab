#include "cache.h"
#include "proxy.h"

CacheNode *newCacheNode(int alloc)
{
    CacheNode *node = (CacheNode *) malloc(sizeof(CacheNode));
    node->url = node->data = node->idx = NULL;
    node->next = node->prev = NULL;
    node->nodeSize = 0;
    node->valid = 1;
    if (alloc) {
        node->url = (char *) malloc(MAXLINE);
        node->data = (char *) malloc(MAX_OBJECT_SIZE);
        node->idx = node->data;
    }
    return node;
}

void deleteCacheNode(CacheNode *node)
{
    free(node->url);
    free(node->data);
    free(node);
}

void setNodeUrl(CacheNode *node, char *url)
{
    strcpy(node->url, url);
}

void appendNodeData(CacheNode *node, char *buf, int cnt)
{
    if (node->nodeSize + cnt > MAX_OBJECT_SIZE) {
        node->valid = 0;
        return;
    }
    memcpy(node->idx, buf, cnt);
    node->nodeSize += cnt;
    node->idx += cnt;
}

void initCacheList(CacheList *list, int maxTotSize)
{
    list->totSize = 0;
    list->maxTotSize = maxTotSize;
    pthread_rwlock_init(&(list->rwlock), NULL);
    list->head = newCacheNode(0);
    list->tail = newCacheNode(0);
    list->head->next = list->tail;
    list->tail->prev = list->head;
}

CacheNode *findNodeByUrl(CacheList *list, char *url)
{
    pthread_rwlock_rdlock(&(list->rwlock));
    CacheNode *cur = list->head->next;
    while (cur != list->tail) {
        if (strcmp(cur->url, url) == 0) {
            break;
        }
        cur = cur->next;
    }

    if (cur == list->tail) {
        cur = NULL;
    }
    pthread_rwlock_unlock(&(list->rwlock));
    return cur;
}

static void removeFromList(CacheNode *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static void evictLastNode(CacheList *list)
{
    CacheNode *lastNode = list->tail->prev;
    list->totSize -= lastNode->nodeSize;
    removeFromList(lastNode);
    deleteCacheNode(lastNode);
}

static void putHead(CacheList *list, CacheNode *node)
{
    CacheNode *head = list->head;
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

void insertNodeToList(CacheList *list, CacheNode *node)
{
    pthread_rwlock_wrlock(&list->rwlock);
    while (list->totSize + node->nodeSize > list->maxTotSize) {
        evictLastNode(list);
    }
    putHead(list, node);
    list->totSize += node->nodeSize;
    pthread_rwlock_unlock(&list->rwlock);
}