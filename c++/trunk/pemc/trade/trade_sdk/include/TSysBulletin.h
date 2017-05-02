#ifndef __TSYSBULLETIN_EVENT_H__
#define __TSYSBULLETIN_EVENT_H__

#include "TradeCommon.h"
#include "TradeEvent.h"
#include "TradeStruct.h"

///ϵͳ�����¼�
/// @brief ��procTradeEvent::EVENT_CODE_SYSBULLETIN ��ص�����Ҫ�漰�������ĸ��๫�����͡�
class SDKDLL_API TSysBulletinEvent: public TradeEvent
{
public:
	TSysBulletinEvent();
	~TSysBulletinEvent();

public:
	void setSysBulletin(SysBulletinInfo info);
	///��ȡ������Ϣ
	SysBulletinInfo  getSysBulletin() const;
private:
	SysBulletinInfo m_info;
};


#endif //__TSYSBULLETIN_EVENT_H__
