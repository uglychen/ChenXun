#ifndef _QUOTE_UTILS_H_
#define _QUOTE_UTILS_H_

#include "QuoteDataDef.h"
#include "zlog/zlog.h"
#include <string>
using std::string;

/** 助手工具类
*/
class QuoteUtils
{

public:
	//根据k线种类时间取整
	static int roundTime(const int time, const KLINE_TYPE& kLineType);

	//根据K线种类获取索引
	static int getIndex(const int tradeDay, const int time, const KLINE_TYPE& kLineType);

	//获取UUID
	static string getUuid();

	//字符串转浮点型
	static double strToDouble(const string& num);

	//比较两个价格 相等返回 0 , 第一个小于第二个返回-1 大于返回1
	static int cmpPrice(const string& first, const string& second);

	//获取数据库Unixtime
	static string getSqlUnixTime(const int timestamp = -1);

	//打印当前时间
	static void printCurTime();

	static string getProductNameById(const string& productId);

	//秒数取整
	static void roundTimeSecond(const int time, int& roundTime);

	//分钟取整
	static int roundTimeMimute(const int time, const int flag);

	//小时取整
	static int roundTimeHour(const int time);

	//根据分钟获取索引
	static void getIndexByMinute(const int tradeDay, const int time, const int flag, int& index);

	//根据小时获取索引
	static void getIndexByHour(const int tradeDay, const int time, const int flag, int& index);

    //格式化时间转时间戳
    static int fmtToTime(const string& fmtTime);

    //时间戳格式化
    static string timeToFmt(const int tm);
};


class MyLog 
{
private:
    MyLog();

public:
    ~MyLog();

    static MyLog& Instance();

    bool Init();
    void UnInit();
};
#endif