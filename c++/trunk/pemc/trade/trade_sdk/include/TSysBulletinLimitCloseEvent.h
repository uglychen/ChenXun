#ifndef __TSYSBULLETIN_LIMITCLOSE_EVENT_H__
#define __TSYSBULLETIN_LIMITCLOSE_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///限价平仓单成交事件
/// @brief 在procTradeEvent::EVENT_CODE_SYSBUL_LIMITCLOSE 里回调。
class SDKDLL_API TSysBulletinLimitCloseEvent: public TradeEvent
{
public:
	TSysBulletinLimitCloseEvent();
	~TSysBulletinLimitCloseEvent();

public:
	void setSysBulletinLimitClose(SysBulletinInfo info);
	///获取消息内容
	SysBulletinInfo  getSysBulletinLimitClose() const;
private:
	SysBulletinInfo m_info;
};


#endif //__TSYSBULLETIN_LIMITCLOSE_EVENT_H__
