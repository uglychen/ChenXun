#ifndef __TREVOKELIMITORDER_EVENT_H__ 
#define __TREVOKELIMITORDER_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///限价单撤销事件
/// @brief 在procTradeEvent::EVENT_CODE_LIMITREVOKE 里回调。
class SDKDLL_API TRevokeLimitOrderEvent: public TradeEvent
{
public:
	TRevokeLimitOrderEvent();
	~TRevokeLimitOrderEvent();

public:
	void setResult(ProcessResult result);
	ProcessResult getResult()const;
	
	void setLimitOrderID(long long LimitOrderID);
	long long getLimitOrderID()const;

private:
	ProcessResult m_Result;
	long long m_LimitOrderID;
};

#endif //__TCLOSELIMITORDER_EVENT_H__

