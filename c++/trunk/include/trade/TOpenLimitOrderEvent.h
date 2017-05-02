#ifndef __TOPENLIMITORDER_EVENT_H__ 
#define __TOPENLIMITORDER_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///限价建仓事件
/// @brief 在procTradeEvent::EVENT_CODE_OPENLIMITORDER 里回调。
class SDKDLL_API TOpenLimitOrderEvent: public TradeEvent
{
public:
	TOpenLimitOrderEvent();
	~TOpenLimitOrderEvent();
public:
	void setResult(ProcessResult result);
	ProcessResult getResult()const;

	void setLimitOrderID(long long LimitOrderID);
	long long getLimitOrderID()const;
private:
	ProcessResult m_Result;
	long long m_LimitOrderID;
};

#endif //__TOPENLIMITORDER_EVENT_H__

