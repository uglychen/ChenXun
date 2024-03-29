/*
 *  tool.cpp
 *
 *  Created on: 2017-4-6
 *  Author: chenxun 275076730@qq.com
 *
 */


#include "tool.h"


std::string getDate()
{
    struct tm *t;
    time_t s;
    time(&s);
    t = localtime(&s);

    char str[32]={0};

    snprintf(str, sizeof(str),
                "%4d-%02d-%02d",
                t->tm_year+1900,
                t->tm_mon+1,
                t->tm_mday
                );
    return str;
}

std::string getDateAndTime()
{   
    struct tm *t;
    time_t s;
    time(&s);
    t = localtime(&s);
    char str[32]={0};
    snprintf(str, sizeof(str),
        "%4d-%02d-%02d %02d%02d%02d",
        t->tm_year+1900,
        t->tm_mon+1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec
        );
        return str;
}








