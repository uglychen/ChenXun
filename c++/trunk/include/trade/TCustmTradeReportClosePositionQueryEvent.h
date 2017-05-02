#ifndef __TCUSTMTRADEREPORTCLOSEPOSITIONQUERY_EVENT_H__ 
#define __TCUSTMTRADEREPORTCLOSEPOSITIONQUERY_EVENT_H__

#include <vector>
#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///�ͻ�ƽ�ֵ������ѯ�¼�
/// @brief ��procTradeEvent::EVENT_CODE_CUSTMTRADEREPORTCLOSEPOSITIONQUERY ��ص���
class SDKDLL_API TCustmTradeReportClosePositionQueryEvent: public TradeEvent
{
public:
	TCustmTradeReportClosePositionQueryEvent();
	~TCustmTradeReportClosePositionQueryEvent();
public:
	void setResult(ProcessResult result);
	void setClosePositionInfoNum(int num);
	ProcessResult getResult()const;
	int getClosePositionInfoNum()const;

public:
///ƽ�ֵ�����������
	std::vector<CustmTradeReportClosePositionInfo> vec;

private:
	ProcessResult m_Result;
	int m_ClosePositionInfoNum;
};

#endif //__TCUSTMTRADEREPORTCLOSEPOSITIONQUERY_EVENT_H__

