#ifndef __TLOGIN_EVENT_H__
#define __TLOGIN_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

///��¼״̬�¼�
/// @brief ��procTradeEvent::EVENT_CODE_LOGIN ��ص����漰��¼�ɹ�/ʧ�ܡ������ߵȡ�
class SDKDLL_API TLoginEvent: public TradeEvent
{
public:
	TLoginEvent();
	~TLoginEvent();

public:
	void setLoginResult(ProcessResult result);
	///��ȡ������
	ProcessResult  getLoginResult() const;
private:
	ProcessResult m_Result;
};


#endif //__TLOGIN_EVENT_H__
