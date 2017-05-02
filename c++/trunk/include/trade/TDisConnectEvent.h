#ifndef __TDISCONNECT_EVENT_H__ 
#define __TDISCONNECT_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///断连事件
/// @brief 在procTradeEvent::EVENT_CODE_DISCONNECT 里回调。
class SDKDLL_API TDisConnectEvent: public TradeEvent
{
public:
	TDisConnectEvent();
	~TDisConnectEvent();
public:
	void setResult(ProcessResult result);
	///获取处理结果
	ProcessResult getResult()const;
private:
	ProcessResult m_Result;

};

#endif //__TDISCONNECT_EVENT_H__



