#ifndef __TCUSTMTRADEREPORTHOLDPOSITIONQUERY_EVENT_H__ 
#define __TCUSTMTRADEREPORTHOLDPOSITIONQUERY_EVENT_H__

#include <vector>
#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///�ͻ��ֲֵ������ѯ�¼�
/// @brief ��procTradeEvent::EVENT_CODE_CUSTMTRADEREPORTHOLDPOSITIONQUERY ��ص���
class SDKDLL_API TCustmTradeReportHoldPositionQueryEvent: public TradeEvent
{
public:
	TCustmTradeReportHoldPositionQueryEvent();
	~TCustmTradeReportHoldPositionQueryEvent();
public:
	void setResult(ProcessResult result);
	void setHoldPositionInfoNum(int num);
	ProcessResult getResult()const;
	int getHoldPositionInfoNum()const;

public:
	///�ֲֵ�����������
	std::vector<CustmTradeReportHoldPositionInfo> vec;

private:
	ProcessResult m_Result;
	int	m_HoldPositionInfoNum;
};

#endif //__TCUSTMTRADEREPORTHOLDPOSITIONQUERY_EVENT_H__

