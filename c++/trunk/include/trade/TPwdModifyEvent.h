#ifndef __TPWDMODIFY_EVENT_H__
#define __TPWDMODIFY_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

///�޸������¼�
/// @brief ��procTradeEvent::EVENT_CODE_PWDMODIFY ��ص���
class SDKDLL_API TPwdModifyEvent: public TradeEvent
{
public:
	TPwdModifyEvent();
	~TPwdModifyEvent();

public:
	void setResult(ProcessResult result);
	ProcessResult  getResult() const;
private:
	ProcessResult m_Result;
};


#endif //__TPWDMODIFY_EVENT_H__
