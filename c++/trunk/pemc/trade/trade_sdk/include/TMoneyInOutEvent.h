#ifndef __TMONEYINOUT_EVENT_H__
#define __TMONEYINOUT_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

///������¼�
/// @brief ��procTradeEvent::EVENT_CODE_MONEYINOUT ��ص���
class SDKDLL_API TMoneyInOutEvent: public TradeEvent
{
public:
	TMoneyInOutEvent();
	~TMoneyInOutEvent();

public:
	void setMoneyInoutResult(MoneyInOutInfo Info);
	///��ȡ������
	MoneyInOutInfo  getMoneyInoutResult() const;
private:
	MoneyInOutInfo m_Info;
};


#endif //__TMONEYINOUT_EVENT_H__
