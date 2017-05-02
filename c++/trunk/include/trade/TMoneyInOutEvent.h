#ifndef __TMONEYINOUT_EVENT_H__
#define __TMONEYINOUT_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

///出入金事件
/// @brief 在procTradeEvent::EVENT_CODE_MONEYINOUT 里回调。
class SDKDLL_API TMoneyInOutEvent: public TradeEvent
{
public:
	TMoneyInOutEvent();
	~TMoneyInOutEvent();

public:
	void setMoneyInoutResult(MoneyInOutInfo Info);
	///获取处理结果
	MoneyInOutInfo  getMoneyInoutResult() const;
private:
	MoneyInOutInfo m_Info;
};


#endif //__TMONEYINOUT_EVENT_H__
