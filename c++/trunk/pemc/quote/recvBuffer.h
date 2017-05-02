#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "list.h"


typedef struct recvBuf {
    uint32_t size;      // data size
    uint32_t tailSize;
    uint32_t len;       // data length
    int recvFd;
    char *head;
    char *tail;
    char data[0];
} recvBuf_t;

recvBuf_t *RBAlloc(uint32_t size);
int RBRecv(recvBuf_t *rb, int fd);
int RBSend(recvBuf_t *rb, listcache_t *lc);
void RBReset(recvBuf_t *rb);






























