#ifndef __TSIGNRESULTNOTIFYQUERY_EVENT_H__
#define __TSIGNRESULTNOTIFYQUERY_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

/// ǩԼ���֪ͨ�¼�
/// @brief ��procTradeEvent::EVENT_CODE_SIGNRESULTNOTIFYQUERY ��ص���
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
