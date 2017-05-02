#ifndef __TSYSBULLETIN_LIQUIDATION_EVENT_H__
#define __TSYSBULLETIN_LIQUIDATION_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///ǿ��ƽ�֣�ն�֣��¼�
/// @brief ��procTradeEvent::EVENT_CODE_SYSBUL_LIQUIDATION ��ص���
class SDKDLL_API TSysBulletinLiquidationEvent: public TradeEvent
{
public:
	TSysBulletinLiquidationEvent();
	~TSysBulletinLiquidationEvent();

public:
	void setSysBulletinLiquidation(SysBulletinInfo info);
	///��ȡ��Ϣ����
	SysBulletinInfo  getSysBulletinLiquidation() const;
private:
	SysBulletinInfo m_info;
};


#endif //__TSYSBULLETIN_LIQUIDATION_EVENT_H__
