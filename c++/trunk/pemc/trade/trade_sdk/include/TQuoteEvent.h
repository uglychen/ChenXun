#ifndef __TQUOTE_EVENT_H__
#define __TQUOTE_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///实时行情接收事件
/// @brief 在procTradeEvent::EVENT_CODE_QUOTE 里回调。
class SDKDLL_API TQuoteEvent: public TradeEvent
{
public:
	TQuoteEvent();
	~TQuoteEvent();

public:
	void setQuote(RealTimeQuote quote);
	///获取行情数据
	RealTimeQuote  getQuote() const;
private:
	RealTimeQuote m_quote;
};


#endif //__TQUOTE_EVENT_H__
