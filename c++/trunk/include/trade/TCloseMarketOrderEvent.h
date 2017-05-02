#ifndef __TCLOSEMARKETORDER_EVENT_H__ 
#define __TCLOSEMARKETORDER_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///市价平仓事件
/// @brief 在procTradeEvent::EVENT_CODE_CLOSEMARKETORDER 里回调。
class SDKDLL_API TCloseMarketOrderEvent: public TradeEvent
{
public:
	TCloseMarketOrderEvent();
	~TCloseMarketOrderEvent();
public:
	void setResult(ProcessResult result);
	///获取处理结果
	ProcessResult getResult()const; 

	void setHoldPositionID(long long nHoldPositionID);
	long long getHoldPositionID() const;
private:
	ProcessResult m_Result;
	long long     m_HoldPositionID;
};

#endif //__TCLOSEMARKETORDER_EVENT_H__


