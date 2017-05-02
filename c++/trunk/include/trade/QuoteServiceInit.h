#ifndef QUOTESERVICEINIT_H
#define QUOTESERVICEINIT_H
#include <stdio.h>
#include <string>

#include "TradeCommon.h"

namespace TradeAPI
{
	namespace business
	{
		class QuoteManager;
	}
}

class SDKDLL_API QuoteServiceInit {
public:
	QuoteServiceInit();
	~QuoteServiceInit();

public:
	int InitQuote(std::string strIP, int nPort);

	int RunQuote(std::string strAppCode, std::string Password);

	int ReleaseQuote();
private:
	TradeAPI::business::QuoteManager *QuoteCommon;
};

#endif //QUOTESERVICEINIT_H
