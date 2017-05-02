#ifndef __TCUSTOMERSELFFUNDFLOWQUERY_EVENT_H__ 
#define __TCUSTOMERSELFFUNDFLOWQUERY_EVENT_H__

#include <vector>
#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///�ͻ�������ѯ�¼�
/// @brief ��procTradeEvent::EVENT_CODE_CUSTOMERSELFFUNDFLOWQUERY ��ص���
class SDKDLL_API TCustomerSelfFundFlowQueryEvent: public TradeEvent
{
public:
	TCustomerSelfFundFlowQueryEvent();
	~TCustomerSelfFundFlowQueryEvent();
public:
	void setResult(ProcessResult result);
	void setFundFlowRecordNum(int num);
	ProcessResult getResult()const;
	int getFundFlowRecordNum()const;

public:
	///�ֲֵ�����������
	std::vector<FundFlowQueryInfo> vec;

private:
	ProcessResult m_Result;
	int	m_FundFlowRecordNum;
};

#endif //__TCUSTOMERSELFFUNDFLOWQUERY_EVENT_H__

