#ifndef __TCUSTMTRADEREPORTLIMITORDERQUERY_EVENT_H__ 
#define __TCUSTMTRADEREPORTLIMITORDERQUERY_EVENT_H__

#include <vector>
#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///�ͻ��޼۵������ѯ�¼�
/// @brief ��procTradeEvent::EVENT_CODE_CUSTMTRADEREPORTLIMITORDERQUERY ��ص���
class SDKDLL_API TCustmTradeReportLimitOrderQueryEvent: public TradeEvent
{
public:
	TCustmTradeReportLimitOrderQueryEvent();
	~TCustmTradeReportLimitOrderQueryEvent();
public:
	void setResult(ProcessResult result);
	void setLimitOrderInfoNum(int num);
	ProcessResult getResult()const;
	int getLimitOrderInfoNum()const;

public:
	///�޼۵�����������
	std::vector<CustmTradeReportLimitOrderInfo> vec;

private:
	ProcessResult m_Result;
	int m_LimitOrderInfoNum;
};

#endif //__TCUSTMTRADEREPORTLIMITORDERQUERY_EVENT_H__

