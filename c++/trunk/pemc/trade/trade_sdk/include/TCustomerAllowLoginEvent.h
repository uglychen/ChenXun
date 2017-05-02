#ifndef __TCUSTOMERALLOWLOGIN_EVENT_H__
#define __TCUSTOMERALLOWLOGIN_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

///�޸Ļ��ѯ�ͻ��Ƿ���Ե�¼
/// @brief ��procTradeEvent::EVENT_CODE_CUSTOMERALLOWLOGIN ��ص���
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
