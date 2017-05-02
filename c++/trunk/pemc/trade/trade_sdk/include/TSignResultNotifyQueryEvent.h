#ifndef __TSIGNRESULTNOTIFYQUERY_EVENT_H__
#define __TSIGNRESULTNOTIFYQUERY_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

/// 签约结果通知事件
/// @brief 在procTradeEvent::EVENT_CODE_SIGNRESULTNOTIFYQUERY 里回调。
class SDKDLL_API TSignResultNotifyQueryEvent: public TradeEvent
{
public:
	TSignResultNotifyQueryEvent();
	~TSignResultNotifyQueryEvent();

public:
	void setResult(ProcessResult result);
	ProcessResult  getResult() const;
private:
	ProcessResult m_Result;	
};


#endif //__TSIGNRESULTNOTIFYQUERY_EVENT_H__
