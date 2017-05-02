#ifndef _QUOTE_CGI_REQUEST_MANAGER_H_
#define _QUOTE_CGI_REQUEST_MANAGER_H_
#include "QuoteCgiResponse.h"
#include <vector>

class QuoteReal;
class KLineManager;
/** CGI网关https请求响应管理 
*/
class QuoteCgiResponseManager
{
public:
	QuoteCgiResponseManager();
	~QuoteCgiResponseManager();

	bool Init();

	void UnInit();

	void SetCallback(QuoteReal* quoteReal, KLineManager* klienManager);
private:

	typedef std::vector<shared_ptr<QuoteCgiResponse>> CgiResponseList;
	CgiResponseList _cgiReponseList;

	//实时行情处理
	QuoteReal* _quoteReal;

	//k线、分时线处理
	KLineManager* _klineManager;
};

#endif