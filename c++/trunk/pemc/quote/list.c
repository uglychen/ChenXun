#include <string.h>
#include <stdlib.h>
#include <zlog.h>
#include "list.h"

static node_t *NodeAlloc(uint32_t size);



static node_t *NodeAlloc(uint32_t size) {
    node_t *node = malloc(sizeof(size));

    memset(node, 0, size);
    node->dataSize = size - sizeof(node_t);

    return node;
}

listcache_t *ListCacheAlloc(uint32_t sum, uint32_t size) {
    listcache_t *lc = malloc(sizeof(listcache_t));

    lc->idle = ListAlloc(sum, size);
    lc->busy = ListAlloc(0, size);

    return lc;
}

list_t *ListAlloc(uint32_t sum, uint32_t size) {
    list_t *list = malloc(sizeof(list_t));

    list->nodeSum = 0;
    list->nodeSize = size;
    list->head = list->tail = NULL;
    if (sum > 0) {
        int i = 0;
        node_t *node = NULL;
        for (i; i < sum; i++)
            ListPutTail(list, NodeAlloc(size));
    }

    return list;
}


bool ListEmpty(list_t *list) {
    return list->nodeSum == 0;
}

void ListPutTail(list_t *list, node_t *node) {
    if (!ListEmpty(list)) {
        list->tail->next = node;
        list->tail = node;
    } else {
        list->head = list->tail = node;
    }
    list->nodeSum++;
    
    dzlog_debug("ListPutTail, list->nodeSum = %d", list->nodeSum);
}

node_t *ListGetHead(list_t *list) {
    node_t *node = list->head;

    if (!ListEmpty(list)) {
        list->head = list->head->next;
        if (list->head == NULL)
            list->tail = NULL;
        list->nodeSum--;
    }

    dzlog_debug("ListGetHead, list->nodeSum = %d", list->nodeSum);
    return node;
}

void NodeSetData(node_t *node, const char *data, uint32_t len) {
    memcpy(node->data, data, len);
    node->datelen = len;
}

void NodeSetFlagPart(node_t *node, uint32_t isPart) {
    node->isPart = isPart;
}

void NodeReset(node_t *node) {
    node->isPart = 0;
    node->datelen = 0;
}














