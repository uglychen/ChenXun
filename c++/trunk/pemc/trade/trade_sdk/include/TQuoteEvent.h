#ifndef __TQUOTE_EVENT_H__
#define __TQUOTE_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///ʵʱ��������¼�
/// @brief ��procTradeEvent::EVENT_CODE_QUOTE ��ص���
class SDKDLL_API TQuoteEvent: public TradeEvent
{
public:
	TQuoteEvent();
	~TQuoteEvent();

public:
	void setQuote(RealTimeQuote quote);
	///��ȡ��������
	RealTimeQuote  getQuote() const;
private:
	RealTimeQuote m_quote;
};


#endif //__TQUOTE_EVENT_H__
