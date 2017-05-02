#ifndef __TLIMITCLOSEPOSITION_EVENT_H__ 
#define __TLIMITCLOSEPOSITION_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///止盈止损设置事件
/// @brief 在procTradeEvent::EVENT_CODE_LIMITCLOSEPOSITION 里回调。
class SDKDLL_API TLimitClosePositionEvent: public TradeEvent
{
public:
	TLimitClosePositionEvent();
	~TLimitClosePositionEvent();
public:
	void setResult(ProcessResult result);
	ProcessResult getResult()const;

	void setHoldPositionID(long long nHoldPositionID);
	long long getHoldPositionID() const;
private:
	ProcessResult m_Result;
	long long     m_HoldPositionID;
};

#endif //__TLIMITCLOSEPOSITION_EVENT_H__

