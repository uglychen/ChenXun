#ifndef _K_LINE_DAY_H_
#define _K_LINE_DAY_H_
#include "KLineBase.h"

/** 日k线 
*/
class KLineDay:
	public KLineBase
{
public:
	KLineDay();
	~KLineDay();
#if 0


	virtual void runQuoteReal(const QuoteMsg& quoteMsg);

	//读取K线历史数据
	virtual void ReadHistoryKLineData();

	//更新k线数据，有更新返回真
	bool CheckUpdateData(KLINE_DATA& klinedata, const int stockTime, const string& lastPrice, const string& hightPrice, const string& lowPrice);

	//发送消息
	void SendUpdate(const string& productId, const KLINE_DATA& klinedata);

	//插入数据库
	void InsertIntoDataBase(const string& productId, const KLINE_DATA& klinedata);

	//检查是否收盘
	bool IsClose(const QuoteMsg& quoteMsg);
private:
	//当前k线时间
	typedef map<string, int> IndexList;
	IndexList _curIndexList;
#endif
};

#endif
