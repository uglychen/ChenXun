#ifndef __TSYSBULLETIN_LIQUIDATION_EVENT_H__
#define __TSYSBULLETIN_LIQUIDATION_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///强制平仓（斩仓）事件
/// @brief 在procTradeEvent::EVENT_CODE_SYSBUL_LIQUIDATION 里回调。
class SDKDLL_API TSysBulletinLiquidationEvent: public TradeEvent
{
public:
	TSysBulletinLiquidationEvent();
	~TSysBulletinLiquidationEvent();

public:
	void setSysBulletinLiquidation(SysBulletinInfo info);
	///获取消息内容
	SysBulletinInfo  getSysBulletinLiquidation() const;
private:
	SysBulletinInfo m_info;
};


#endif //__TSYSBULLETIN_LIQUIDATION_EVENT_H__
