#ifndef __TCUSTMTRADEREPORTHOLDPOSITIONQUERY_EVENT_H__ 
#define __TCUSTMTRADEREPORTHOLDPOSITIONQUERY_EVENT_H__

#include <vector>
#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///客户持仓单报表查询事件
/// @brief 在procTradeEvent::EVENT_CODE_CUSTMTRADEREPORTHOLDPOSITIONQUERY 里回调。
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
	///持仓单报表结果数据
	std::vector<CustmTradeReportHoldPositionInfo> vec;

private:
	ProcessResult m_Result;
	int	m_HoldPositionInfoNum;
};

#endif //__TCUSTMTRADEREPORTHOLDPOSITIONQUERY_EVENT_H__

