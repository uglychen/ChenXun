#ifndef __TCUSTOMERSELFFUNDFLOWQUERY_EVENT_H__ 
#define __TCUSTOMERSELFFUNDFLOWQUERY_EVENT_H__

#include <vector>
#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///客户出入金查询事件
/// @brief 在procTradeEvent::EVENT_CODE_CUSTOMERSELFFUNDFLOWQUERY 里回调。
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
	///持仓单报表结果数据
	std::vector<FundFlowQueryInfo> vec;

private:
	ProcessResult m_Result;
	int	m_FundFlowRecordNum;
};

#endif //__TCUSTOMERSELFFUNDFLOWQUERY_EVENT_H__

