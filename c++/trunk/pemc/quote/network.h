#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>

#define NET_OK (0)
#define NET_ERR (-1)
#define NET_EAGAIN (-2)

int netCreateSocket(int domain, int type);
int netSetReuseAddr(int fd);
int netConnect(int fd, char *destip, uint16_t destport);
int netRead(int fd, char *buf, int count);
int netWrite(int fd, char *buf, int count);
uint32_t ipatoi(const char *ip);


































