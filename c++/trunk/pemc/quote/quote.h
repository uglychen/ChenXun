#pragma once
#include <stdio.h>

#define THREAD_CHECK_TIME 60

#define RET_SUCCESS (0)
#define RET_FAIL (-1)

#define NULL_MSG_LENGTH 12
#define MSG_HEAD_LENGTH 11

#define MSG_TYPE_NULL 0
#define MSG_TYPE_QUOTE 1
#define MSG_TYPE_HEARTBEAT 2


int verifyHeadFlag(const char *data);
int verifyCmdCode(const char *data, const char *code);
int verifyBodyLength(const char *data, const char *bodylen);
int verifyMsgVersion(const char *data);
int verifyTailFlag(const char *data, int off);
int getBodyLength(const char *data);
int getMsgType(const char *head);
int verifyQuoteHead(const char *head);
int verifyQuoteBody(const char *body, int bodylen);
int verifyHeartbeat(const char *head);






















