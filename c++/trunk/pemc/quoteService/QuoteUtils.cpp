#include "QuoteUtils.h"
#include "KLineConfig.h"
#include "Product_def.h"
#include <uuid/uuid.h>  
#include <time.h>
#include <iostream>
#include <math.h>
#include <string.h>
#include <iconv.h> 
#include <sys/stat.h>
#include<sys/types.h>
#include <unistd.h>
using namespace std;

int QuoteUtils::roundTime(const int time, const KLINE_TYPE& kLineType)
{
	switch (kLineType)
	{
	case KLINE_TYPE_1MIN:
	case KLINE_TYPE_5MIN:
	case KLINE_TYPE_15MIN:
	case KLINE_TYPE_30MIN:
	{
		int flag = KLineConfig::getMinFlag(kLineType);
		return roundTimeMimute(time, flag);
		break;
	}
	case KLINE_TYPE_60MIN:
		return roundTimeHour(time);
		break;
	default:
		break;
	}

	return 1;
}


int QuoteUtils::getIndex(const int tradeDay, const int time, const KLINE_TYPE& kLineType)
{
	int flag = KLineConfig::getMinFlag(kLineType);
	int index = -1;
	switch (kLineType)
	{
	case KLINE_TYPE_1MIN:
	case KLINE_TYPE_5MIN:
	case KLINE_TYPE_15MIN:
	case KLINE_TYPE_30MIN:
	{
		
		getIndexByMinute(tradeDay, time, flag, index);
		break;
	}
	case KLINE_TYPE_60MIN:
		getIndexByHour(tradeDay, time, flag, index);
		break;
	default:
		break;
	}

	return index;
}

string QuoteUtils::getUuid()
{
	char cc[33] = { 0 };
	uuid_t uu;
	uuid_generate(uu);

	for (int i = 0; i < 16; i++)
	{
		sprintf(&cc[i * 2], "%02x", uu[i]);
	}

	return string(cc);
}

void QuoteUtils::roundTimeSecond(const int time, int& roundTime)
{
	time_t timep = time;
	struct tm p;
	memset(&p, 0, sizeof(p));

	localtime_r(&timep, &p);
	p.tm_sec = 0;
	roundTime = mktime(&p);
}

int QuoteUtils::roundTimeMimute(const int time, const int flag)
{
	if (flag <= 0 || flag >= 60 )
	{
		std::cout << "flag err!!!!!!" << std::endl;
		return -1;
	}

	time_t timep = time;
	struct tm p;
	memset(&p, 0, sizeof(p));
	localtime_r(&timep, &p);
	p.tm_sec = 0;
	p.tm_min = p.tm_min / flag * flag;

	return mktime(&p);
}

int QuoteUtils::roundTimeHour(const int time)
{
	time_t timep = time;
	struct tm p;
	memset(&p, 0, sizeof(p));
	localtime_r(&timep, &p);
	p.tm_sec = 0;
	p.tm_min = 0;
	return mktime(&p);
}

void QuoteUtils::getIndexByMinute(const int tradeDay, const int time, const int flag, int& index)
{
	if (flag <= 0 || flag >= 60)
	{
		std::cout << "flag err!!!!!!x" << std::endl;
		return;
	}

	//起始小时标记
	const int startHour = 5;

	time_t timeTradeDay = tradeDay;
	struct tm tTradeDay;
	memset(&tTradeDay, 0, sizeof(tTradeDay));
	localtime_r(&timeTradeDay, &tTradeDay);
	tTradeDay.tm_yday;

	time_t timep = time;
	struct tm p;
	memset(&p, 0, sizeof(p));
	localtime_r(&timep, &p);
	p.tm_sec = 0;
	p.tm_min = p.tm_min / flag * flag;

	int dayInterval = p.tm_yday - tTradeDay.tm_yday;
	if (dayInterval < 0 || dayInterval > 1)
	{
		if (flag <= 0 || flag >= 60)
		{
			std::cout << "flag err!!!!!!xx" << std::endl;
			return;
		}
	}

	int hourInterval = (dayInterval * 24 + p.tm_hour) - startHour;
	int totalMinute = hourInterval * 60 + p.tm_min;
	index = totalMinute / flag;
}


void QuoteUtils::getIndexByHour(const int tradeDay, const int time, const int flag, int& index)
{
	if (flag <= 0 || flag >= 24)
	{
		std::cout << "hour flag err!!!!!!x" << std::endl;
		return;
	}

	//起始小时标记
	const int startHour = 5;

	time_t timeTradeDay = tradeDay;
	struct tm tTradeDay;
	memset(&tTradeDay, 0, sizeof(tTradeDay));
	localtime_r(&timeTradeDay, &tTradeDay);
	tTradeDay.tm_yday;

	time_t timep = time;
	struct tm p;
	memset(&p, 0, sizeof(p));
	localtime_r(&timep, &p);
	int dayInterval = p.tm_yday - tTradeDay.tm_yday;
	if (dayInterval < 0 || dayInterval > 1)
	{
		if (flag <= 0 || flag >= 60)
		{
			std::cout << "flag err!!!!!!xx" << std::endl;
			return;
		}
	}

	int hourInterval = (dayInterval * 24 + p.tm_hour) - startHour;
	index = hourInterval / flag;
}


int QuoteUtils::fmtToTime(const string& fmtTime)
{
    struct tm p2;
    memset(&p2, 0, sizeof(p2));
    strptime(fmtTime.c_str(), "%Y-%m-%d %H:%M:%S", &p2);
    return mktime(&p2);
}


string QuoteUtils::timeToFmt(const int tm)
{
    time_t tmp = tm;
    struct tm p;
    memset(&p, 0, sizeof(p));
    localtime_r(&tmp, &p);
    char s[100] = { 0 };
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &p);

    return s;
}

double QuoteUtils::strToDouble(const string& num)
{
	//C++ 11 -std=c++11
	return stod(num);
}

int QuoteUtils::cmpPrice(const string& first, const string& second)
{
	if (first == second || first.empty() || second.empty())
	{
		return 0;
	}

	double dbFirst = stod(first);
	double dbSecond = stod(second);

#if 0 //启用之前要确定好精度！
	if (fabs(dbFirst - dbSecond) < 0.000001)
	{
		return 0;
	}
#endif
	
	return dbFirst > dbSecond ? 1 : -1;
}

string QuoteUtils::getSqlUnixTime(const int timestamp /*= -1*/)
{
	char temp[1024] = { 0 };
	int tmp = timestamp;
	if (tmp == -1)
	{
		time_t now;
		time(&now);
		tmp = now;
	}
	sprintf(temp, "FROM_UNIXTIME(%d)", tmp);
	return string(temp);
}

void QuoteUtils::printCurTime()
{
	time_t now;
	time(&now);

	cout << ctime(&now) << endl;
}

string QuoteUtils::getProductNameById(const string& productId)
{
	if (productId == PRODUCT_ID_SLIVER)
	{
		return PRODUCT_NAME_SLIVER;
	}
	else if (productId == PRODUCT_ID_PALLADIUM)
	{
		return PRODUCT_NAME_PALLADIUM;
	}
	else if (productId == PRODUCT_ID_PLATINUM)
	{
		return PRODUCT_NAME_PLATINUM;
	}

	return "";
}

/*
int QuoteUtils::GetHour()
{
    time_t tmp;
    tmp = time((time_t*)NULL);
    //std::cout << tmp << std::endl;
    struct tm p;
    memset(&p, 0, sizeof(p));
    localtime_r(&tmp, &p);
    //std::cout << "hour:" << p.tm_hour << std::endl;
    return p.tm_hour;
}*/

MyLog::MyLog()
{

}

MyLog::~MyLog()
{
    UnInit();
}

MyLog& MyLog::Instance()
{
    static MyLog mylog;
    return mylog;
}

bool MyLog::Init()
{
    const string logFolder = "./log";
    if (access(logFolder.c_str(), 0) == -1)//access函数是查看文件是不是存在
    {
        if (mkdir(logFolder.c_str(), 0777))//如果不存在就用mkdir函数来创建
        {
            cout << "创建日志文件夹失败！" << endl;
            return false;
        }
    }

    int rc;
    rc = dzlog_init("zlog.conf", "my_cat");

    if (rc) 
    {
        cout << "zlog 初始化失败！" << endl;
        return false;
    }

    dzlog_info("zlog 初始化成功！");
    return true;
}

void MyLog::UnInit()
{
    zlog_fini();
}
