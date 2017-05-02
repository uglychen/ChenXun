#ifndef __TSYSBULLETIN_EVENT_H__
#define __TSYSBULLETIN_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///系统公告事件
/// @brief 在procTradeEvent::EVENT_CODE_SYSBULLETIN 里回调。主要涉及交易所的各类公告推送。
class SDKDLL_API TSysBulletinEvent: public TradeEvent
{
public:
	TSysBulletinEvent();
	~TSysBulletinEvent();

public:
	void setSysBulletin(SysBulletinInfo info);
	///获取公告信息
	SysBulletinInfo  getSysBulletin() const;
private:
	SysBulletinInfo m_info;
};


#endif //__TSYSBULLETIN_EVENT_H__
