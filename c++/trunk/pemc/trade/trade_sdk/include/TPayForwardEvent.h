#ifndef __TPAYFORWARD_EVENT_H__
#define __TPAYFORWARD_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

/// 签约结果通知事件
/// @brief 在procTradeEvent::EVENT_CODE_PAYFORWARD 里回调。
class SDKDLL_API TPayForwardEvent: public TradeEvent
{
public:
	TPayForwardEvent();
	~TPayForwardEvent();

public:
	void setResult(ProcessResult result);
	ProcessResult  getResult() const;
private:
	ProcessResult m_Result;	
};


#endif //__TPAYFORWARD_EVENT_H__
