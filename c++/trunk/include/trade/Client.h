#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "TradeCommon.h"

class TradeEvent;

///��׼�ͻ���
///@brief main������ʵ��������ʵ�ֻص�����procTradeEvent�������ԱTradeInterface���󣬵���TradeInterface�ӿڽ��н��ײ�����
class SDKDLL_API Client
{
public:
	///�ص����������ڽ����������͡�TradeInterface����Ĵ������ͷ��ص����ݡ���TradeEventCode.h
	virtual void procTradeEvent(const TradeEvent* pEvent) = 0;
	virtual ~Client(){}
};


#endif //__CLIENT_H__



