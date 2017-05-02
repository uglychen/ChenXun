#ifndef __TPAYFORWARD_EVENT_H__
#define __TPAYFORWARD_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

/// ǩԼ���֪ͨ�¼�
/// @brief ��procTradeEvent::EVENT_CODE_PAYFORWARD ��ص���
class SDKDLL_API TPayForwardEvent: public TradeEvent
{
public:
	TPayForwardEvent();
	~TPayForwardEvent();

public:
	void setResult(ProcessResult result);
	ProcessResult  getResult() const;
private:
	ProcessResult m_Result;	
};


#endif //__TPAYFORWARD_EVENT_H__
