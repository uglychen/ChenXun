#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlog.h>
#include "quote.h"
#include "network.h"
#include "recvBuffer.h"

recvBuf_t *RBAlloc(uint32_t size) {
    recvBuf_t *rb = malloc(size);
    
    memset(rb, 0, sizeof(recvBuf_t));
    rb->size = size - sizeof(recvBuf_t);
    rb->tailSize = rb->size;
    rb->recvFd = -1;
    rb->head = rb->tail = rb->data;

    return rb;
}

int RBRecv(recvBuf_t *rb, int fd) {
    int recvLen;

    recvLen = netRead(fd, rb->tail, rb->tailSize);
    if (recvLen > 0) {
        rb->tail += recvLen;
        rb->len += recvLen;
        rb->tailSize -= recvLen;
        dzlog_debug("RBRecv success, len = %d, tailSize = %d!", rb->len, rb->tailSize);
        return RET_SUCCESS;
    } else {
        return RET_FAIL;
    }
}

int RBSend(recvBuf_t *rb, listcache_t *lc) {
    int bodyLen = 0, msgLen = 0, msgType;
    node_t *node = NULL;

    if (!ListEmpty(lc->idle)) {
        while (1) {
            if (rb->len > MSG_HEAD_LENGTH) {
                bodyLen = getBodyLength(rb->head);
                msgLen = NULL_MSG_LENGTH + bodyLen;
                if (rb->len >= msgLen) {
                    msgType = getMsgType(rb->head);
                    if (msgType == MSG_TYPE_QUOTE) {
                        if ((verifyQuoteHead(rb->head) == RET_SUCCESS) && 
                            (verifyQuoteBody(rb->head + MSG_HEAD_LENGTH, bodyLen) == RET_SUCCESS)) {
                            node = ListGetHead(lc->idle);
                            if (bodyLen <= node->dataSize) {
                                NodeSetData(node, rb->head + MSG_HEAD_LENGTH, bodyLen);
                                ListPutTail(lc->busy, node);
                                // TODO: 1/2 move data to head
                            } else {
                                dzlog_info("RBSend a large msg!");
                                // TODO: sava msg
                            }

                            alarm(30);
                        } else {
                            return RET_FAIL;
                        }
                    } else if (msgType == MSG_TYPE_HEARTBEAT) {
                        alarm(30);
                        dzlog_debug("RBSend, msg is a heartbeat, drop it!");
                        
                    } else {
                        dzlog_error("RBSend fail, invalid msg type!");
                        return RET_FAIL;
                    }

                    rb->len -= msgLen;
                    rb->head += msgLen;
                    if (rb->len == 0) RBReset(rb);
                    dzlog_debug("RBSend success, len = %d, tailSize = %d!", rb->len, rb->tailSize);
                    continue;
                } else {
                    dzlog_debug("RBSend, no a whole msg!");
                }
            } else {
                dzlog_debug("RBSend, no a whole head!");
            }

            return RET_SUCCESS;
        }
    } else {
        dzlog_error("RBSend fail, no idle node to use!");
        return RET_FAIL;
    }
 
}

void RBReset(recvBuf_t *rb) {
    rb->len = 0;
    rb->head = rb->tail = rb->data;
    rb->tailSize = rb->size;
    dzlog_debug("RBReset!");
}




























