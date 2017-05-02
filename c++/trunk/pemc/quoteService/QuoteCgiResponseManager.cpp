#include "QuoteCgiResponseManager.h"
#include "Product_def.h"

QuoteCgiResponseManager::QuoteCgiResponseManager():
	_quoteReal(NULL),
	_klineManager(NULL)
{

}

QuoteCgiResponseManager::~QuoteCgiResponseManager()
{

}

bool QuoteCgiResponseManager::Init()
{
	if (_quoteReal == NULL || _klineManager == NULL)
	{
		return false;
	}

	_cgiReponseList.clear();
	const int numOfReponse = 8;
	for (int i = 0; i < numOfReponse; i++)
	{
		_cgiReponseList.push_back(shared_ptr<QuoteCgiResponse>(new QuoteCgiResponse));
		_cgiReponseList[i]->SetCallback(_quoteReal, _klineManager);
		_cgiReponseList[i]->Init(BROKER_URI, CGI_REQUEST_QUEUE);
		
	}
	return true;
}

void QuoteCgiResponseManager::SetCallback(QuoteReal* quoteReal, KLineManager* klienManager)
{
	_quoteReal = quoteReal;
	_klineManager = klienManager;
}
