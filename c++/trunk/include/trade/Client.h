#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "TradeCommon.h"

class TradeEvent;

///标准客户类
///@brief main函数里实例化，并实现回调函数procTradeEvent；增添成员TradeInterface对象，调用TradeInterface接口进行交易操作。
class SDKDLL_API Client
{
public:
	///回调函数。用于接收行情推送、TradeInterface对象的处理结果和返回的数据。见TradeEventCode.h
	virtual void procTradeEvent(const TradeEvent* pEvent) = 0;
	virtual ~Client(){}
};


#endif //__CLIENT_H__



