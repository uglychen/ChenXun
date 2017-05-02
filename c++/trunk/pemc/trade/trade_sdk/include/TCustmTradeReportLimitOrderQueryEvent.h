#ifndef __TCUSTMTRADEREPORTLIMITORDERQUERY_EVENT_H__ 
#define __TCUSTMTRADEREPORTLIMITORDERQUERY_EVENT_H__

#include <vector>
#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///客户限价单报表查询事件
/// @brief 在procTradeEvent::EVENT_CODE_CUSTMTRADEREPORTLIMITORDERQUERY 里回调。
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
	///限价单报表结果数据
	std::vector<CustmTradeReportLimitOrderInfo> vec;

private:
	ProcessResult m_Result;
	int m_LimitOrderInfoNum;
};

#endif //__TCUSTMTRADEREPORTLIMITORDERQUERY_EVENT_H__

