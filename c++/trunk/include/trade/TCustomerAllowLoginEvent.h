#ifndef __TCUSTOMERALLOWLOGIN_EVENT_H__
#define __TCUSTOMERALLOWLOGIN_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

///修改或查询客户是否可以登录
/// @brief 在procTradeEvent::EVENT_CODE_CUSTOMERALLOWLOGIN 里回调。
class SDKDLL_API TCustomerAllowLoginEvent: public TradeEvent
{
public:
	TCustomerAllowLoginEvent();
	~TCustomerAllowLoginEvent();

public:
	void setAllowLoginInfo(AllowLoginInfo info);
	AllowLoginInfo  getAllowLoginInfo() const;
private:
	AllowLoginInfo m_Info;
};


#endif //__TCUSTOMERALLOWLOGIN_EVENT_H__
