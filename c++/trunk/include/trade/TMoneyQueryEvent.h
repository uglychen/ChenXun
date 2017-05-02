#ifndef __TMONEYQUERY_EVENT_H__
#define __TMONEYQUERY_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///可用资金查询事件
/// @brief 在procTradeEvent::EVENT_CODE_MONEYQUERY 里回调。获取BourseMoneyInfo。
class SDKDLL_API TMoneyQueryEvent: public TradeEvent
{
public:
	TMoneyQueryEvent();
	~TMoneyQueryEvent();

public:
	void setResult(ProcessResult result);
	ProcessResult  getResult() const;

	void setBourseMoneyInfo(BourseMoneyInfo bourseMoneyInfo);
	///获取可用资金
	BourseMoneyInfo  getBourseMoneyInfo() const;

private:
	ProcessResult   m_result;
	BourseMoneyInfo	m_bourseMoneyInfo;
};


#endif //__TMONEYQUERY_EVENT_H__
