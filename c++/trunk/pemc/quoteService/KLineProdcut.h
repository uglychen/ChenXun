#ifndef _K_LINE_PRODUCT_H_
#define _K_LINE_PRODUCT_H_
#include "QuoteDataDef.h"
#include "xn_rwlock.h"
#include <jsoncpp/json.h>
#include <map>

using namespace std;
class MqttPublish;

struct KlineInfo
{
	string _open;
	string _close;
	string _high;
	string _low;
};


struct KlineInfo2
{
	KlineInfo2():
		_stockTime(0)
	{}
	int _stockTime;
	string _open;
	string _close;
	string _high;
	string _low;
};


/** 一种商品的K线处理
*/
class KLineProdcut 
{
public:

	KLineProdcut();

	~KLineProdcut();

	//初始化数据
	void InitData(const string& productId, const KLINE_TYPE& klineType, IKLineCallBack* pKlineCallBack);

	//处理行情数据
	void ProcessProductQuote(const QuoteMsg& quoteMsg);

    void GetKLineData(const string& startTime, Json::Value& data);
private:
	//检查交易日是否变化
	void CheckTradeDate(const int tradeDate);

	//交易日发生变化
	void OnTradeDateChange();

	//处理k线数据
	void ProcessKlineData(const QuoteMsg& quoteMsg);

	//检查行情是否发生变化（时间、价格），如果有变化推送到MQTT上
	void CheckQuoteChange(const QuoteMsg& quoteMsg);

	//交易行情发生变化
	void OnQuoteChange();

	//插入数据
	void InsertIntoData(const KlineInfo2& klineInfo);
	//更新数据
	void UpdateData(const KlineInfo2& klineInfo);

	//重置
	void Reset();

	//插入数据到数据库
	void InsertIntoDataBase(const KlineInfo2& klineInfo);

	//更新数据到数据库
	void UpdateDataBase(const KlineInfo2& klineInfo);

	//读取历史数据
	bool LoadHisKlineData();
private:

	//商品种类
	string _productId;

	//k线种类
	KLINE_TYPE _klineType;

	//当前交易日
	int _curTradeDate;

	//读写锁
	xn_rwlock _rwlock;

	//key:时间戳
	//vlaue:k线数据
	typedef map<int, KlineInfo> KlineInfoList;
	KlineInfoList _klineInfolist;

	//当前价格数据
	KlineInfo2 _curKlineInfo;

	//上次时间戳
	int _lastStockTime;
	//上次价格
	string _lastPrice;

	//k线回调对象
	IKLineCallBack* _pKlineCallback;

	//主题名字
	string _topicName;

	//数据库表名字
	string _tableName;

    //历史数据时间
    int _dayInterval;

    //json 数据
    Json::Value _jsData;
};
#endif