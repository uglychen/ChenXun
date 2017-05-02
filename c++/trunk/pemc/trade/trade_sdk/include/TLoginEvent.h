#ifndef __TLOGIN_EVENT_H__
#define __TLOGIN_EVENT_H__

#include "TradeCommon.h"
#include "TradeStruct.h"
#include "TradeEvent.h"

///登录状态事件
/// @brief 在procTradeEvent::EVENT_CODE_LOGIN 里回调。涉及登录成功/失败、踢下线等。
class SDKDLL_API TLoginEvent: public TradeEvent
{
public:
	TLoginEvent();
	~TLoginEvent();

public:
	void setLoginResult(ProcessResult result);
	///获取处理结果
	ProcessResult  getLoginResult() const;
private:
	ProcessResult m_Result;
};


#endif //__TLOGIN_EVENT_H__
