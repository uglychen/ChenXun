#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <zlog.h>
#include "network.h"
#include "recvBuffer.h"
#include "crc8.h"
#include "quote.h"

#define QUOTE_SERVER_IP "183.62.250.18"
#define QUOTE_SERVER_PORT 10088
#define QUOTE_SERVER_PASSWORD "PMEC180"

#define CODE_LOGIN_REQUEST "0204"
#define CODE_LOGIN_RESPONSE_FAIL "0100"
#define CODE_LOGIN_RESPONSE_SUCCESS "0200"
#define CODE_DATA "0201"
#define CODE_HEARTBEAT "0203"
#define VERSION "02"

static void daemonize(void);
static void initZlog(void);
static int connectQuoteServer(void);
static void *handleJson(void *param);
static void *handleQuote(void *param);
static bool isThreadAlive(pthread_t tid);
static int connectServer(void);
static int sendLoginRequest(int fd);
static int verifyLoginResponse(int fd);
static void recvQuotePackage(int fd);
static void handleHeartBeat(int signo);


static recvBuf_t *gRecvBuf;
static listcache_t *gListCache;
pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv) {
	int ret;
	pthread_t tidJson, tidQuote;

    daemonize();
    initZlog();

    dzlog_debug("main thread start!");
    gRecvBuf = RBAlloc(4096*1024);
    gListCache = ListCacheAlloc(8*1024, 512);
	pthread_create(&tidJson, NULL, handleJson, NULL);
	pthread_create(&tidQuote, NULL, handleQuote, NULL);

	while (1) {
		if (!isThreadAlive(tidJson)) pthread_create(&tidJson, NULL, handleJson, NULL);
		if (!isThreadAlive(tidQuote)) pthread_create(&tidQuote, NULL, handleQuote, NULL);

		sleep(10);
	}
}

static void daemonize() {
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
}

static void initZlog() {
    char *zlogfile = "/opt/xncp/zlog.conf", *zlogcat = "quote";

    if (dzlog_init(zlogfile, zlogcat)) exit(1);
}

static void *handleJson(void *param) {
    dzlog_info("thread handleJson start!");

	while (1) {

    }

    dzlog_error("thread handleJson quit!");
    return NULL;
}

static int connectQuoteServer() {
	int fd;

    fd = netCreateSocket(AF_INET, SOCK_STREAM);
    netSetReuseAddr(fd);
    if (netConnect(fd, QUOTE_SERVER_IP, QUOTE_SERVER_PORT) == NET_OK) {
        return fd;
    } else {
        close(fd);
        return RET_FAIL;
    }
}

static void recvQuotePackage(int fd) {
    while (1) {
        if (RBRecv(gRecvBuf, fd) == RET_SUCCESS && RBSend(gRecvBuf, gListCache) == RET_SUCCESS) {
        } else {
            RBReset(gRecvBuf);
            break;
        }
    }
}

static void *handleQuote(void *param) {
	int fd;
	char buff[NULL_MSG_LENGTH] = {0};

    dzlog_info("thread handleQuote start!");
	while (1) {
        memset(buff, 0, NULL_MSG_LENGTH);
		fd = connectQuoteServer();
		if (fd != RET_FAIL) {
			if (sendLoginRequest(fd) == RET_SUCCESS) {
                if (verifyLoginResponse(fd) == RET_SUCCESS) {
                    gRecvBuf->recvFd = fd;
                    signal(SIGALRM, handleHeartBeat);
                    alarm(30);
                    recvQuotePackage(fd);
                    close(fd);
                    signal(SIGALRM, SIG_IGN);

                    sleep(1);
                    continue;
                }
			}
			close(fd);
            sleep(1);
		} else {
			break;
		}

	}

	dzlog_error("thread handleQuote quit!");
    return NULL;
}

static bool isThreadAlive(pthread_t tid) {
	int ret = pthread_kill(tid, 0);
	return ret == 0;
}

static int sendLoginRequest(int fd) {
	int sendLen;
	size_t pwdLen = strlen(QUOTE_SERVER_PASSWORD);
	char loginPkg[19] = {0};

	*loginPkg = 0xff;
	strcpy(loginPkg + 1, CODE_LOGIN_REQUEST);
	sprintf(loginPkg + 5, "%04d", (int)pwdLen);
	strcpy(loginPkg + 9, VERSION);
	strcpy(loginPkg + 11, QUOTE_SERVER_PASSWORD);

    sendLen = netWrite(fd, loginPkg, 19);
	if (sendLen == 19) {
		dzlog_info("send login package success, %s!", loginPkg);
		return RET_SUCCESS;
	} else {
        dzlog_error("send login package fail!");
        return RET_FAIL;
    }
}

static int verifyLoginResponse(int fd) {
    int recvLen;
    char loginResponse[NULL_MSG_LENGTH] = {0};

    recvLen = netRead(fd, loginResponse, NULL_MSG_LENGTH);
    if (recvLen == NULL_MSG_LENGTH) {
        if (verifyHeadFlag(loginResponse) == RET_SUCCESS &&
            verifyCmdCode(loginResponse, CODE_LOGIN_RESPONSE_SUCCESS) == RET_SUCCESS &&
            verifyBodyLength(loginResponse, "0000") == RET_SUCCESS &&
            verifyMsgVersion(loginResponse) == RET_SUCCESS &&
            verifyTailFlag(loginResponse, 11) == RET_SUCCESS) {
            dzlog_info("verifyLoginResponse success!");
            return RET_SUCCESS;
        }
    }

    return RET_FAIL;
}

static void handleHeartBeat(int signo) {
    dzlog_info("server timeout, need reconnect!");
    //close(gRecvBuf->recvFd);
    shutdown(gRecvBuf->recvFd, SHUT_RDWR);
}

int verifyHeadFlag(const char *data) {
    if ((uint8_t)(*data) == 255) {
        return RET_SUCCESS;
    } else {
        dzlog_error("verifyHeadFlag fail!");
        return RET_FAIL;
    }
}

int verifyCmdCode(const char *data, const char *code) {
	if (strncmp(data + 1, code, 4) == 0) {
        return RET_SUCCESS;
    } else {
        dzlog_error("verifyCmdCode fail!");
        return RET_FAIL;
    }
}

int verifyBodyLength(const char *data, const char *bodylen) {
	if (strncmp(data + 5, bodylen, 4) == 0) {
        return RET_SUCCESS;
    } else {
        dzlog_error("verifyBodyLength fail!");
        return RET_FAIL;
    }
}

int verifyMsgVersion(const char *data) {
	if (strncmp(data + 9, VERSION, 2) == 0) {
        return RET_SUCCESS;
    } else {
        dzlog_error("verifyMsgVersion fail!");
        return RET_FAIL;
    }
}

int verifyTailFlag(const char *data, int off) {
	if (*(data + off) == 0) {
        return RET_SUCCESS;
    } else {
        dzlog_error("verifyTailFlag fail!");
        return RET_FAIL;
    }
}

int getBodyLength(const char *data) {
    int len = 0, i = 0;
    char *offdata = (char *)data + 5;

    for (i; i < 3; i++) {
        len += (*offdata) - '0';
        len *= 10;
        offdata++;
    }
    len += (*offdata) - '0';

    dzlog_debug("getBodyLength: %d!", len);
    return len;
}

int verifyQuoteHead(const char *head) {
    if (verifyHeadFlag(head) == RET_SUCCESS &&
        verifyCmdCode(head, CODE_DATA) == RET_SUCCESS &&
        verifyMsgVersion(head) == RET_SUCCESS) {
        dzlog_debug("verifyQuoteHead success!");
        return RET_SUCCESS;
    } else {
        dzlog_error("verifyQuoteHead fail!");
        return RET_FAIL;
    }
}

int verifyQuoteBody(const char *body, int bodylen) {
    dzlog_debug("verifyQuoteBody, %d, %d", getCRC8(body, bodylen), (uint8_t)(*(body + bodylen)));
    if (getCRC8(body, bodylen) == (uint8_t)(*(body + bodylen))) {
        dzlog_debug("verifyQuoteBody success!");
        return RET_SUCCESS;
    } else {
        dzlog_error("verifyQuoteBody fail!");
        return RET_FAIL;
    }
}

int getMsgType(const char *head) {
    if (strncmp(head + 1, CODE_DATA, 4) == 0) {
        dzlog_debug("getMsgType, quote data!");
        return MSG_TYPE_QUOTE;
    } else if (strncmp(head + 1, CODE_HEARTBEAT, 4) == 0) {
        dzlog_debug("getMsgType, heartbeat!");
        return MSG_TYPE_HEARTBEAT;
    } else {
        dzlog_error("getMsgType, null!");
        return MSG_TYPE_NULL;
    }
}

