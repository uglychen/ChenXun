#ifndef __TOPENDELIVERYORDER_EVENT_H__ 
#define __TOPENDELIVERYORDER_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

/// 交割申请事件
/// @brief 在procTradeEvent::EVENT_CODE_OPENDELIVERYORDER 里回调。
class SDKDLL_API TOpenDeliveryOrderEvent: public TradeEvent
{
public:
	TOpenDeliveryOrderEvent();
	~TOpenDeliveryOrderEvent();
public:
	void setResult(ProcessResult result);
	ProcessResult getResult()const;

	void setHoldPositionID(long long nHoldPositionID);
	long long getHoldPositionID() const;
private:
	ProcessResult m_Result;
	long long     m_HoldPositionID;
};

#endif //__TOPENDELIVERYORDER_EVENT_H__

