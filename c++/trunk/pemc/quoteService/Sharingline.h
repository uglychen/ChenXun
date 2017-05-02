#ifndef _SHARING_LINE_H_
#define _SHARING_LINE_H_
#include "QuoteDataDef.h"
#include "Quote_buffer.h"
#include "MqttPublish.h"
#include "xn_rwlock.h"
#include <vector>
#include <map>
#include <string>
#include <pthread.h>
#include <unistd.h>

using namespace std;

//分时线数据类
class ShareLineData
{
public:
    ShareLineData():_stockTime(0){}

public:
    int _stockTime;
    string _closePrice;
};

class SharingLine
{
public:
    SharingLine(ShareLineType shareLineType, const string& address, const string& clientId);
    virtual ~SharingLine();

    //更新实时行情
    virtual void runQuoteReal(const QuoteMsg& quoteMsg);

    //初始化分时线数据
    virtual void InitShareLineInfo();

    //读取分时线数据
    //virtual void ReadHistoryShareLineData();

    //把数据放到消息队列中去
    void PushQuoteMsg(const QuoteMsg& quoteMsg);

    void Init();
    void InitBase();
    void UnInitBase();
    
    //发送消息0
    void SendUpdate(const string& productId, const ShareLineData& klinedata);

    //插入数据库
    void InsertIntoDataBase(const string& productId, const ShareLineData& klinedata);

    //更新数据
    void updateShareLineDB(const string& productId, ShareLineData& data);

    //执行sql语句
    void ExecuteDb(const string& str);

    //把从mq中收到的数据存到map中去
    void insertMap(const QuoteMsg& quoteMsg);

    //获取某种产品分时数据
    void GetShareLineData(const std::string& productId, std::map<int, ShareLineData>& data_map);

protected:
    static void* proc_thread_func(void *param);

protected:
    //产品列表
    typedef map<string, ProdcutInfo> ProductInfoList;
    ProductInfoList _prodcutList;

    //分时线种类
    ShareLineType _shareLineType;

    //当天分时数据保存在map中
    typedef std::map<int, ShareLineData> SharingLineDataMap;
    typedef std::map<std::string, SharingLineDataMap> ProdcutShareLineDataList;
    ProdcutShareLineDataList _curTradeDayData;
    std::map<std::string, int> time_map;

    //历史交易日的数据
    ProdcutShareLineDataList _hisKLineData;

    //mqtt 消息发布对象
    MqttPublish _mqttPulish;

    //行情缓冲区
    Quote_buffer _quoteBuffer;

    pthread_t _thread_id;
    volatile bool _run;

private:
    const int _flag = 1;    //分钟标记
    int _curTradeDate;      //当前交易日
    xn_rwlock _rwlock;      //读写锁
};

#endif
