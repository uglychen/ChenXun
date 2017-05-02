#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct node {
    struct node *next;
    uint32_t isPart;    // >0 not a whole msg; =0 a whole msg
    uint32_t dataSize;
    uint32_t datelen;
    char data[0];
} node_t;

typedef struct list {
    uint32_t nodeSum;
    uint32_t nodeSize;
    node_t *head;
    node_t *tail;
} list_t;

typedef struct listcache {
    list_t *idle;
    list_t *busy;
} listcache_t;

listcache_t *ListCacheAlloc(uint32_t sum, uint32_t size);
void ListPutTail(list_t *list, node_t *node);
node_t *ListGetHead(list_t *list);
bool ListEmpty(list_t *list);
list_t *ListAlloc(uint32_t sum, uint32_t size);
void NodeSetData(node_t *node, const char *data, uint32_t len);
void NodeSetFlagPart(node_t *node, uint32_t isPart);
void NodeReset(node_t *node);
