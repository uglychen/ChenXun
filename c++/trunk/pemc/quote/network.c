#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <execinfo.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <zlog.h>
#include "network.h"


int netCreateSocket(int domain, int type) {
    int s;
    if ((s = socket(domain, type, 0)) == -1) {
        dzlog_error("netCreateSocket: %s", strerror(errno));
        return NET_ERR;
    }
    return s;
}

int netSetReuseAddr(int fd) {
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        dzlog_error("netSetReuseAddr: %s", strerror(errno));
        return NET_ERR;
    }
    return NET_OK;
}

int netConnect(int fd, char *destip, uint16_t destport) {
    struct sockaddr_in destAddr;
    memset((void*)(&destAddr), 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_addr.s_addr = htonl(ipatoi(destip));
    destAddr.sin_port = htons(destport);
    if (connect(fd, (struct sockaddr *)&destAddr, sizeof(destAddr)) == -1 && errno != EINPROGRESS) {
        dzlog_error("netConnect %s:%u fail, %s", destip, destport, strerror(errno));
        return NET_ERR;
    }

    dzlog_info("netConnect %s:%u success", destip, destport);
    return NET_OK;
}

int netRead(int fd, char *buf, int count) {
    ssize_t recvLen;

    recvLen = recv(fd, buf, count, 0);
    if (recvLen > 0) {
        dzlog_debug("netRead success, recvLen = %u", (uint32_t)recvLen);
    } else if (recvLen == 0) {
        dzlog_info("netRead recv FIN, close connection");
    } else if (recvLen == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            dzlog_debug("netRead fail, %s, wait for next event", strerror(errno));
            recvLen = NET_EAGAIN;
        } else {
            dzlog_error("netRead fail, %s, close connection", strerror(errno));
            recvLen = NET_ERR;
        }
    }

    return recvLen;
}

int netWrite(int fd, char *buf, int count) {
    ssize_t sendLen, off = 0;

    while (off < count) {
        sendLen = send(fd, buf + off, count - off, 0);
        if (sendLen >= 0) {
            off += sendLen;
            dzlog_debug("netWrite success, sendLen = %u", (uint32_t)sendLen);
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                dzlog_debug("netWrite fail, try to write again in next loop");
                return NET_EAGAIN;
            } else if (errno == EINTR) {
                continue;
            } else {
                dzlog_error("netWrite fail: %s", strerror(errno));
                return NET_ERR;
            }     
        }
    }

    return count;
}

uint32_t ipatoi(const char *ip) {
    in_addr_t ipi;

    ipi = inet_addr(ip);
    return ntohl(ipi);
}

