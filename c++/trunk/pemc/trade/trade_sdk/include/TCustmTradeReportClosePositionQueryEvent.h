#ifndef __TCUSTMTRADEREPORTCLOSEPOSITIONQUERY_EVENT_H__ 
#define __TCUSTMTRADEREPORTCLOSEPOSITIONQUERY_EVENT_H__

#include <vector>
#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///客户平仓单报表查询事件
/// @brief 在procTradeEvent::EVENT_CODE_CUSTMTRADEREPORTCLOSEPOSITIONQUERY 里回调。
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
///平仓单报表结果数据
	std::vector<CustmTradeReportClosePositionInfo> vec;

private:
	ProcessResult m_Result;
	int m_ClosePositionInfoNum;
};

#endif //__TCUSTMTRADEREPORTCLOSEPOSITIONQUERY_EVENT_H__

