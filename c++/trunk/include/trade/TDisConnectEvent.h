#ifndef __TDISCONNECT_EVENT_H__ 
#define __TDISCONNECT_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///�����¼�
/// @brief ��procTradeEvent::EVENT_CODE_DISCONNECT ��ص���
class SDKDLL_API TDisConnectEvent: public TradeEvent
{
public:
	TDisConnectEvent();
	~TDisConnectEvent();
public:
	void setResult(ProcessResult result);
	///��ȡ������
	ProcessResult getResult()const;
private:
	ProcessResult m_Result;

};

#endif //__TDISCONNECT_EVENT_H__



