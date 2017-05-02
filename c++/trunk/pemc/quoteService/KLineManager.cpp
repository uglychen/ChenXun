#include "KLineManager.h"
#include "KLineMinute.h"
#include "Sharingline.h"
#include "KLineDay.h"
#include "Product_def.h"
#include <memory>
KLineManager::KLineManager()
{
}

KLineManager::~KLineManager()
{
	UnInit();
}

string KLineManager::runQuoteReal(const QuoteMsg& quoteMsg)
{
	return "";
}

void KLineManager::Init()
{
	_kLineList.clear();

	/*shared_ptr<KLineBase> sp5Min(new KLineBase(KLINE_TYPE_5MIN, ADDRESS, CLIENTID_KLINE_5MIN));
	shared_ptr<KLineBase> sp15Min(new KLineBase(KLINE_TYPE_15MIN, ADDRESS, CLIENTID_KLINE_15MIN));
	shared_ptr<KLineBase> sp30Min(new KLineBase(KLINE_TYPE_30MIN, ADDRESS, CLIENTID_KLINE_30MIN));
	shared_ptr<KLineBase> sp60Min(new KLineBase(KLINE_TYPE_60MIN, ADDRESS, CLIENTID_KLINE_60MIN));
	//shared_ptr<KLineBase> sp1day(new KLineDay());
	_kLineList[KLINE_TYPE_TEXT_5MIN] = sp5Min;
	_kLineList[KLINE_TYPE_TEXT_15MIN] = sp15Min;
	_kLineList[KLINE_TYPE_TEXT_30MIN] = sp30Min;
 	_kLineList[KLINE_TYPE_TEXT_60MIN] = sp60Min;
	//_kLineList[KLINE_TYPE_TEXT_1DAY] = sp1day;
*/

	KLineList::iterator ibeg = _kLineList.begin();
	KLineList::iterator iend = _kLineList.end();
	for (; ibeg != iend; ibeg++)
	{
		ibeg->second->Init();
	}

	_ptrShareLine.reset(new SharingLine(SHARE_TYPE_1MIN, ADDRESS, CLIENTID_SHARE_1MIN));
	_ptrShareLine->Init();

}

void KLineManager::UnInit()
{
	_kLineList.clear();
}

void KLineManager::PushQuoteMsg(const QuoteMsg& quoteMsg)
{
	KLineList::iterator ibeg = _kLineList.begin();
	KLineList::iterator iend = _kLineList.end();
	for (; ibeg != iend; ibeg++)
	{
		ibeg->second->PushQuoteMsg(quoteMsg);
	}

	if (_ptrShareLine != NULL)
	{
		_ptrShareLine->PushQuoteMsg(quoteMsg);
	}
	
	return;
}

void KLineManager::GetKLineData(const string& productId, const string& kLineType, const string& startTime, Json::Value& data)
{
    KLineList::iterator pos = _kLineList.find(kLineType);
    if (pos != _kLineList.end())
    {
        pos->second->GetKLineData(productId, startTime, data);
    }
}

void KLineManager::GetShareLineDate(const string& productId, std::map<int, ShareLineData>& tmp_map)
{
    if (_ptrShareLine != NULL)
    {
        _ptrShareLine->GetShareLineData(productId, tmp_map);
    }
    else
    {
        cout << "error!!!" << endl;
    }
    
}



