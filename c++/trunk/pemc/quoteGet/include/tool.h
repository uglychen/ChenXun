/*
 *  tool.h
 *
 *  Created on: 2017-4-6
 *  Author: chenxun 275076730@qq.com
 *
 */


#ifndef _H_TOOL_H_
#define _H_TOOL_H_

#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <uuid/uuid.h>
#include <stdio.h>
#include "zlog.h"
#include "BlockQueue.h"

#include "json/json.h"
#include <uuid/uuid.h>

extern BlockedQueue<std::string> g_list;

extern int rc;
extern zlog_category_t *zc;
// global zlog  variable
void zlogInit(std::string path);

std::string getDate();
std::string getDateAndTime();

void saveFile(const std::string& data);

#endif


