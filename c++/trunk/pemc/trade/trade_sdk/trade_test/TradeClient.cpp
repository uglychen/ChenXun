

// TradeClient.cpp : 定义控制台应用程序的入口点。
//

#include "TradeClient.h"
#include <signal.h>
#include <errno.h>

std::string getime()
{
	char dest[32] = {0};
	time_t sec;
	time(&sec);
#ifdef WIN32
	struct tm result;
	localtime_s(&result , &sec);
	_snprintf(dest, sizeof(dest), "%02d%02d%02d%02d:000000", result.tm_mday, result.tm_hour, result.tm_min, result.tm_sec);
#else
	strftime(dest, sizeof(dest), "%d%H%M%S:000000", localtime(&sec));
#endif
	string tmp = dest;
	return tmp;
}


TradeClient::TradeClient()
{
	m_Interface = NULL;
	gStatus = -99;
}

TradeClient::~TradeClient()
{

}


int TradeClient::TimeToStr(const long long second, char* dest, int len)
{
	if(dest == NULL || len < 20)
		return -1;

	memset(dest, 0, len);

    time_t sec = second;
	if(sec < TS_1970_TO_1900)
	{
#ifdef WIN32
		_snprintf(dest, len, "1900-01-01 00:00:00");
#else
		snprintf(dest, len, "1900-01-01 00:00:00");
#endif
		return 1;
	}
    sec -= TS_1970_TO_1900;

#ifdef WIN32
	struct tm result;
    localtime_s(&result , &sec);
    _snprintf(dest, len, "%04d-%02d-%02d %02d:%02d:%02d", result.tm_year + 1900, result.tm_mon + 1,
            result.tm_mday, result.tm_hour, result.tm_min, result.tm_sec);
#else
    strftime(dest, len, "%Y-%m-%d %H:%M:%S", localtime(&sec));
#endif

	return 0;
}

int TradeClient::Init()
{
	if(m_Interface != NULL)
	{
		printf("[TE] m_Interface had been new!\n");
		return 0;
	}

	if (!m_bMutex.IsInitialized())
	{
		if(m_bMutex.Init() < 0)
		{
			return -2;
		}
	}

	m_Interface = new TradeInterface;
	m_MoneyStatus = MSG_STATUS_NO_UPDATE;

	int ret = InitNetwork();
	if(ret != 0)
	{
		delete m_Interface;
		m_Interface = NULL;
		return ret;
	}

	gStatus = 0;
	last = time(NULL);
	return ret;
}

int TradeClient::InitNetwork()
{
	int ret = m_Interface->Init("127.0.0.1",10086,"./"); // 根据提供的IP更改
	if(0 != ret){
		printf("[TE] init network fail !\n");
		return ret;
	}
	m_Interface->RegisterClient(this);
	m_Interface->SetAppMessage("test1","123456");
	m_Interface->setVendorID("874987497234");

	return ret;
}

int TradeClient::Login(std::string ploginacc, std::string ppwd)
{
	// to get the LoginAccount & pwd from the InputText
	string loginacc = ploginacc;
	string pwd = ppwd;
	LoginAcc = ploginacc;

	int ret = -1;
	ret = m_Interface->doLogin(loginacc,pwd);
	if(0 == ret){
		printf("[T] login succeed !\n");
		gStatus = 1;
		
		StartHeartBeat();

		return ret;
	}else{
		printf("[TE] TradeClient::Login >> login fail! ret:%d\n",ret);
		//sleep(1);
		//printf("[T] Now retry Login! time:%d\n",rt+1);
		//Init();
	}
	return ret;
}

int TradeClient::LoginPwdUpd(std::string oldpwd, std::string newpwd, int markettype)
{
	m_bMutex.Lock();
	int ret = m_Interface->doLoginPwdUpd(oldpwd, newpwd, markettype);
	m_bMutex.Unlock();
	if(0 != ret){
		printf("[TE] LoginPwdUpd fail! ret: %d\n",ret);
		return ret;
	}

	return ret;
}

int TradeClient::FundPwdUpd(std::string oldpwd, std::string newpwd)
{
	m_bMutex.Lock();
	int ret = m_Interface->doFundPwdUpd(oldpwd, newpwd);
	m_bMutex.Unlock();
	if(0 != ret){
		printf("[TE] FundPwdUpd fail! ret: %d\n",ret);
		return ret;
	}

	return ret;
}

int TradeClient::Logout()
{
	return -1;
}

 
int TradeClient::Release()
{
	int ret = -1;
	if(m_Interface)
	{
		ret = m_Interface->Release();
		if(ret == -1)
			return ret;
		delete m_Interface;
		m_Interface = NULL;
		gStatus = -99;
	}

	return ret;	
}


void TradeClient::procTradeEvent(const TradeEvent *pEvent)
{

	switch(pEvent->GetType())
	{
		case EVENT_CODE_LOGIN:
		{
			const TLoginEvent *p  = (const TLoginEvent*)(pEvent);
			ProcessResult result = p->getLoginResult();
			if(Event_Ret_Success != result.RetCode)
			{
				if(Event_Ret_DROPUSER == result.RetCode)
				{
					printf("[TE] ACC[%s] You Have Logon At Other Place! Ret:%d, Msg:%s\n",LoginAcc.c_str(),result.RetCode, result.Message);
					gStatus = -10;
					//Release();
				}
				else if(Event_Ret_LOGACCERR == result.RetCode)
				{
					printf("[TE] ACC[%s] LOGINACCOUNT Not Exist! Ret:%d, Msg:%s\n",LoginAcc.c_str(),result.RetCode, result.Message);
					gStatus = -10;
					//Re lease();
				}
				printf( "[TE] ACC[%s] Login Fail! Ret:%d, Msg:%s\n",LoginAcc.c_str(),result.RetCode, result.Message);
			}
			break;
		}
		case EVENT_CODE_PWDMODIFY:
		{
			const TPwdModifyEvent *p  = (const TPwdModifyEvent*)(pEvent);
			ProcessResult result = p->getResult();
			if(Event_Ret_Success != result.RetCode)
			{
				printf( "[TE] ACC[%s] Modify Pwd Fail! Ret:%d, Msg:%s\n",LoginAcc.c_str(),result.RetCode, result.Message);
			}   
			break;
		}
		case EVENT_CODE_CUSTOMERALLOWLOGIN:
		{
			const TCustomerAllowLoginEvent *p  = (const TCustomerAllowLoginEvent*)(pEvent);
			AllowLoginInfo info = p->getAllowLoginInfo();
			if(Event_Ret_Success != info.RetCode)
			{
				printf( "[TE] ACC[%s] CUSTOMERALLOWLOGIN Fail! Ret:%d, Msg:%s\n",LoginAcc.c_str(), info.RetCode, info.Message);      
			}
			break;
		}
		case EVENT_CODE_QUOTE:
		{
			////printf("get quote retSignal!\n");
			const TQuoteEvent *p = (const TQuoteEvent*)(pEvent);
			RealTimeQuote quote =  p->getQuote();
			//printf("[T] ACC[%s] QUOTE: %d,%f,%f,%f,%f,%lld\n", LoginAcc.c_str(),quote.CommodityID,quote.BuyPrice,quote.SellPrice,quote.HighPrice,quote.LowPrice,quote.QuoteTime);
			break;
		}
		case EVENT_CODE_OPENMARKETORDER:
		{
			printf("[T] [%s] [%s] get OPENMARKETORDER retSignal !\n", getime().c_str(), LoginAcc.c_str());

			const TOpenMarketOrderEvent *e=NULL;
			e = (const TOpenMarketOrderEvent *)pEvent;
			if(Event_Ret_Success != e->getResult().RetCode)
			{
				printf("[TE] OPENMARKETORDER ERROR! result code = %d, msg = %s\n",e->getResult().RetCode, e->getResult().Message);
			}
			else
			{
			}

			break;
		}
		case EVENT_CODE_CLOSEMARKETORDER:
		{
			printf("[T] [%s] [%s] get CLOSEMARKETORDER retSignal !\n",getime().c_str(), LoginAcc.c_str());

			const TCloseMarketOrderEvent *e=NULL;
			e = (const TCloseMarketOrderEvent *)pEvent;
			if(Event_Ret_Success != e->getResult().RetCode)
			{
			    printf("[TE] CLOSEMARKETORDER ERROR! result code = %d, msg = %s\n",e->getResult().RetCode,e->getResult().Message);
			}
			else
			{
			}

			break;
		}
		case EVENT_CODE_OPENLIMITORDER:
		{
			printf("[T] [%s] [%s] get OPENLIMITORDER retSignal !\n",getime().c_str(), LoginAcc.c_str());

			const TOpenLimitOrderEvent *e=NULL;
			e = (const TOpenLimitOrderEvent *)pEvent;
			if(Event_Ret_Success != e->getResult().RetCode)
			{
			    printf("[TE] OPENLIMITORDER ERROR! result code = %d, msg = %s\n",e->getResult().RetCode, e->getResult().Message);
			}
			else
			{
			}
			break;
		}
		case EVENT_CODE_LIMITCLOSEPOSITION:
		{
			printf("[T] [%s] [%s] get CLOSELIMITORDER retSignal !\n",getime().c_str(), LoginAcc.c_str());

			const TLimitClosePositionEvent *e=NULL;
			e = (const TLimitClosePositionEvent *)pEvent;
			if(Event_Ret_Success != e->getResult().RetCode)
			{
				printf("[TE] CLOSELIMITORDER ERROR! result code = %d, msg = %s\n",e->getResult().RetCode, e->getResult().Message);
			}
			else
			{
			}
			break;
		}
		case EVENT_CODE_OPENDELIVERYORDER:
		{
			printf("[T] [%s] [%s] get OPENDELIVERYORDER retSignal !\n",getime().c_str(), LoginAcc.c_str());
			const TOpenDeliveryOrderEvent *e = (const TOpenDeliveryOrderEvent*)(pEvent);
			if(Event_Ret_Success != e->getResult().RetCode)
			{
				printf("[TE] OPENDELIVERYORDER ERROR! result code = %d, msg = %s\n",e->getResult().RetCode, e->getResult().Message);
			}else
			{
			}
			break;
		}
		case EVENT_CODE_CUSTMTRADEREPORTHOLDPOSITIONQUERY:
		{
			printf("[T] [%s] [%s] get REPORTHOLDPOSITIONQUERY retSignal !\n",getime().c_str(), LoginAcc.c_str());

			const TCustmTradeReportHoldPositionQueryEvent *e=NULL;
			e = (const TCustmTradeReportHoldPositionQueryEvent *)pEvent;
			if(Event_Ret_Success != e->getResult().RetCode)
			{
				printf("[TE] REPORTHOLDPOSITIONQUERY ERROR! result code = %d, msg = %s\n",e->getResult().RetCode, e->getResult().Message);   
			}
			else
			{
				printf("[TE] REPORTHOLDPOSITIONQUERY total num[%d]!\n", e->getHoldPositionInfoNum());
				std::vector<CustmTradeReportHoldPositionInfo>::const_iterator it;
				for(it=e->vec.begin(); it!=e->vec.end(); it++)
				{
					printf("[TE] REPORTHOLDPOSITION:建仓单号[%lld], 建仓时间[%lld], 商品[%s], 持仓数量[%d], 持仓总量[%lf], 买/卖[%d], 建仓价[%lf], 持仓价[%lf], 手续费[%lf], 滞纳金[%lf], 履约准备金[%lf], 建仓数量[%d], 建仓总量[%lf], 结算盈亏[%lf], 结算价[%lf]\n", it->holdpositionid, it->opendate, it->commodityname, it->holdquantity, it->holdweight, it->opendirector, it->openprice, it->holdpositionpric, it->commission, it->latefee, it->perfmargin, it->openquantity, it->openweight, it->settlementpl, it->settleprice);
				}
			}
			break;
		}
		case EVENT_CODE_CUSTMTRADEREPORTCLOSEPOSITIONQUERY:
		{
			printf("[T] [%s] [%s] get REPORTCLOSEPOSITIONQUERY retSignal !\n",getime().c_str(), LoginAcc.c_str());

			const TCustmTradeReportClosePositionQueryEvent *e=NULL;
			e = (const TCustmTradeReportClosePositionQueryEvent *)pEvent;
			if(Event_Ret_Success != e->getResult().RetCode)
			{
				printf("[TE] REPORTCLOSEPOSITIONQUERY ERROR! result code = %d, msg = %s\n",e->getResult().RetCode, e->getResult().Message);   
			}
			else
			{
				printf("[TE] REPORTCLOSEPOSITIONQUERY total num[%d]!\n", e->getClosePositionInfoNum());
				std::vector<CustmTradeReportClosePositionInfo>::const_iterator it;
				for(it=e->vec.begin(); it!=e->vec.end(); it++)
				{
					printf("[TE] REPORTCLOSEPOSITION:建仓单号[%lld], 建仓时间[%lld], 商品[%s], 数量[%d], 总量[%lf], 买/卖[%d], 建仓价[%lf], 持仓价[%lf], 平仓单号[%lld], 平仓时间[%lld], 买/卖[%d], 平仓价[%lf], 平仓手续费[%lf], 建仓手续费[%lf], 盈亏[%lf]\n", it->holdpositionid, it->opendate, it->commodityname, it->closequantity, it->closeweight, it->opendirector, it->openprice, it->holdpositionpric, it->closepositionid, it->closedate, it->closedirector, it->closeprice, it->commission, it->opencommission , it->profitorloss);
				}
			}   
			break;
		}   
		case EVENT_CODE_CUSTMTRADEREPORTLIMITORDERQUERY:
		{
			printf("[T] [%s] [%s] get REPORTLIMITORDERQUERY retSignal !\n",getime().c_str(), LoginAcc.c_str());

			const TCustmTradeReportLimitOrderQueryEvent *e=NULL;
			e = (const TCustmTradeReportLimitOrderQueryEvent *)pEvent;
			if(Event_Ret_Success != e->getResult().RetCode)
			{
				printf("[TE] REPORTLIMITORDERQUERY ERROR! result code = %d, msg = %s\n",e->getResult().RetCode, e->getResult().Message);   
			}
			else
			{
				printf("[TE] REPORTLIMITORDERQUERY total num[%d]!\n", e->getLimitOrderInfoNum());
				std::vector<CustmTradeReportLimitOrderInfo>::const_iterator it;
				for(it=e->vec.begin(); it!=e->vec.end(); it++)
				{
					printf("[TE] REPORTLIMITORDER:限价单号[%lld], 下单时间[%lld], 商品[%s], 数量[%d], 总量[%lf], 买/卖[%d], 类型[%d], 限价[%lf], 止损价[%lf], 止盈价[%lf], 期限[%d], 冻结准备金[%lf]\n", it->limitorderid, it->createdate, it->commodityname, it->openquantity, it->openweight, it->opendirector, it->limittype, it->orderprice, it->slprice, it->tpprice, it->deadline, it->frozenreserve);
				}
			}   
			break;
		}
		case EVENT_CODE_CUSTOMERSELFFUNDFLOWQUERY:
		{
			printf("[T] [%s] [%s] get CUSTOMERSELFFUNDFLOW retSignal !\n",getime().c_str(), LoginAcc.c_str());

			const TCustomerSelfFundFlowQueryEvent *e=NULL;
			e = (const TCustomerSelfFundFlowQueryEvent *)pEvent;
			if(Event_Ret_Success != e->getResult().RetCode)
			{
				printf("[TE] CUSTOMERSELFFUNDFLOW ERROR! result code = %d, msg = %s\n",e->getResult().RetCode, e->getResult().Message);
			}   
			else
			{
				printf("[TE] CUSTOMERSELFFUNDFLOW total num[%d]!\n", e->getFundFlowRecordNum());
				std::vector<FundFlowQueryInfo>::const_iterator it;
				for(it=e->vec.begin(); it!=e->vec.end(); it++)
				{
					printf("[TE] SELFFUNDFLOW:流水号[%d], 操作类型[%d], 起始金额[%lf], 变动金额[%lf], 变后金额[%lf], 操作员[%s], 日期[%lld], 银行标识[%d], 关联单号[%d], 红包标识[%s]\n", it->FlowNumber, it->OperType, it->BeforeAmount, it->Amount, it->AfterAmount, it->OpLoginAccount, it->OperDate, it->BankID, it->OrderID, it->Remark);
				}
			}
			break;
		}
		case EVENT_CODE_LIMITREVOKE:
		{
			printf("[T] [%s] [%s] get LIMITREVOKE retSignal !\n",getime().c_str(), LoginAcc.c_str());

			const TRevokeLimitOrderEvent *e=NULL;
			e = (const TRevokeLimitOrderEvent *)pEvent;
			if(Event_Ret_Success != e->getResult().RetCode)
			{
				printf("[TE] LIMITREVOKE ERROR! result code = %d, msg = %s\n",e->getResult().RetCode, e->getResult().Message);
			}
			else
			{
			}

			break;
		}
		case EVENT_CODE_DISCONNECT:
		{
			printf("[T] [%s] [%s] get DISCONNECT retSignal !\n",getime().c_str(), LoginAcc.c_str());

			const TDisConnectEvent *e = (const TDisConnectEvent*)(pEvent);
			if(Event_Ret_Success != e->getResult().RetCode)
			{
				printf("[TE] ACC[%s] DISCONNECT ERROR! result code = %d, msg = %s\n",LoginAcc.c_str(),e->getResult().RetCode, e->getResult().Message);
				gStatus = -20;
				//Release();
			}
			else
			{
			}
			break;
		}
		case EVENT_CODE_SYSBULLETIN:
		{
			printf("get SYSBULLETIN!\n");

			const TSysBulletinEvent *p = (const TSysBulletinEvent*)(pEvent);
			SysBulletinInfo mSysBulletinInfo = p->getSysBulletin();

			printf("[T] LoginID:%d, BulletinID:%lld, TradeMode:%d, BulletinType:%d, BulletinPriorty:%d\n",mSysBulletinInfo.LoginID,\
					mSysBulletinInfo.BulletinID, mSysBulletinInfo.TradeMode, mSysBulletinInfo.BulletinType, mSysBulletinInfo.BulletinPriorty);
			printf("BulletinHeader:%s, BulletinContent:%s\n",mSysBulletinInfo.BulletinHeader, mSysBulletinInfo.BulletinContent);
			printf("PublisherName:%s, BulletinMethod:%d\n",mSysBulletinInfo.PublisherName, mSysBulletinInfo.BulletinMethod);

			break;
		}
		case EVENT_CODE_SYSBUL_LIMITCLOSE:
		{
			printf("get SYSBULLETIN_LIMITCLOSE!\n");
			const TSysBulletinEvent *p = (const TSysBulletinEvent*)(pEvent);
			SysBulletinInfo mSysBulletinInfo = p->getSysBulletin();

			printf("[T] LoginID:%d, BulletinID:%lld, TradeMode:%d, BulletinType:%d, BulletinPriorty:%d\n",mSysBulletinInfo.LoginID,\
				mSysBulletinInfo.BulletinID, mSysBulletinInfo .TradeMode, mSysBulletinInfo.BulletinType, mSysBulletinInfo.BulletinPriorty);
			printf("BulletinHeader:%s, BulletinContent:%s\n",mSysBulletinInfo.BulletinHeader, mSysBulletinInfo.BulletinContent);
			printf("PublisherName:%s, BulletinMethod:%d\n",mSysBulletinInfo.PublisherName, mSysBulletinInfo.BulletinMethod);
			break;
		}
		case EVENT_CODE_SYSBUL_LIQUIDATION:
		{
			printf("get SYSBULLETIN_LIQUIDATION!\n");
			const TSysBulletinEvent *p = (const TSysBulletinEvent*)(pEvent);
			SysBulletinInfo mSysBulletinInfo = p->getSysBulletin();
		
			printf("[T] LoginID:%d, BulletinID:%lld, TradeMode:%d, BulletinType:%d, BulletinPriorty:%d\n",mSysBulletinInfo.LoginID,\
					mSysBulletinInfo.BulletinID, mSysBulletinInfo.TradeMode, mSysBulletinInfo.BulletinType, mSysBulletinInfo.BulletinPriorty);
			printf("BulletinHeader:%s, BulletinContent:%s\n",mSysBulletinInfo.BulletinHeader, mSysBulletinInfo.BulletinContent);
			printf("PublisherName:%s, BulletinMethod:%d\n",mSysBulletinInfo.PublisherName, mSysBulletinInfo.BulletinMethod);
			break;
		}
		case EVENT_CODE_MONEYINOUT:
		{

			printf("get MONEY_INOUT!\n");
			const TMoneyInOutEvent *e = (const TMoneyInOutEvent*)(pEvent);
			MoneyInOutInfo mMoneyInOutInfo = e->getMoneyInoutResult();
			if(Event_Ret_Success != mMoneyInOutInfo.RetCode)
			{
				printf("[TE] MONEYINOUT ERROR! result code = %d, msg = %s\n", mMoneyInOutInfo.RetCode, mMoneyInOutInfo.Message);
			}
			printf("[T] MONEYINOUT Info localsid = %s, banksid = %s, notifyurl = %s, ticket=%s\n", mMoneyInOutInfo.LocalSid, mMoneyInOutInfo.BankSid, mMoneyInOutInfo.NotifyUrl, mMoneyInOutInfo.Ticket);
			break;
		}
		case EVENT_CODE_SIGNRESULTNOTIFYQUERY:
		{
			printf("get SIGNRESULTNOTIFYQUERY !\n");
			const TSignResultNotifyQueryEvent *p = (const TSignResultNotifyQueryEvent*)(pEvent);
			ProcessResult result = p->getResult();
			if(Event_Ret_Success != result.RetCode)
			{
				printf( "[TE] ACC[%s] SignResultNotifyQuery Fail! Ret:%d, Msg:%s\n",LoginAcc.c_str(),result.RetCode, result.Message);
			}
			else
			{
				printf( "[TE] ACC[%s] SignResultNotifyQuery Sucess!\n", LoginAcc.c_str());
			}
			break;
		}
		case EVENT_CODE_PAYFORWARD:
		{
			printf("get PAYFORWARD !\n");
			const TPayForwardEvent *p = (const TPayForwardEvent*)(pEvent);
			ProcessResult result = p->getResult();
			if(Event_Ret_Success != result.RetCode)
			{
				printf( "[TE] ACC[%s] PayForward Fail! Ret:%d, Msg:%s\n",LoginAcc.c_str(),result.RetCode, result.Message);
			}
			else
			{
				printf( "[TE] ACC[%s] PayForward Sucess!\n", LoginAcc.c_str());
			}
			break;
		}
		case EVENT_CODE_MONEYQUERY:
		{
			printf("[T] [%s] ACC[%s] \t-----------\t---------\t----------\t-----------\t get MONEY_QUERY!\n", getime().c_str(), LoginAcc.c_str());
			const TMoneyQueryEvent *e = (const TMoneyQueryEvent*)(pEvent);
			if(Event_Ret_Success != e->getResult().RetCode)
			{

				printf("[TE] MONEYQUERY ERROR! result code = %d\n",e->getResult().RetCode);
			}
			else
			{

				//memcpy(&m_BourseMoneyInfo,&(e->getBourseMoneyInfo()),sizeof(BourseMoneyInfo));
				m_BourseMoneyInfo = e->getBourseMoneyInfo();
				printf("[T] Amount:%-10.3f, AmountAvailable:%-10.3f, AmountFetchable:%-10.3f\n",m_BourseMoneyInfo.Amount,m_BourseMoneyInfo.AmountAvailable,m_BourseMoneyInfo.AmountFetchable);
				m_MoneyStatus = MSG_STATUS_UPDATED;
			}

			break;
		}
		case EVENT_CODE_HEARTBEAT:
		{

			//printf("[T] [%s] ACC[%s] Keep HeartBeat!!\n",getime().c_str(), LoginAcc.c_str());
			break;
		}
		default:
		{

			printf("[TE] get unknow RETEVENT:%d\n", pEvent->GetType());
			break;
		}
	}

}

int TradeClient::OnCustomerAllowLogin(AllowLoginParam mParam)
{
	m_bMutex.Lock();
	int ret = m_Interface->doCustomerAllowLogin(mParam);
	m_bMutex.Unlock();
	if(0 != ret){
		printf("[TE] doCustomerAllowLogin fail! ret: %d\n",ret);
	}

	return ret;
}

int TradeClient::GetAccountInfo()
{

	AccountInfo account;

	int ret = m_Interface->getAccountInfo(account);

	if(MSG_STATUS_UPDATED != ret){

		printf("[TE] GetAccountInfo fail! ret: %d\n",ret);
		return ret;
	}

	printf("[T] Account=%s\nLoginAccount=%s\nCustomerName=%s\n \
			Amount:%f\tOpenProfit:%f\tFrozenReserve:%f\tRiskRate:%f\n \
			NAVPrice:%f\tPerformanceReserve:%f\n", \
			account.Account, account.LoginAccount, account.CustomerName,  \
			account.Amount, account.OpenProfit, account.FrozenReserve, account.RiskRate,  \
			account.NAVPrice, account.PerformanceReserve);

	return ret;
}

int TradeClient::GetMarketStatus()
{
	int marketstatus;
	m_Interface->getMarketStatus(marketstatus);

	if(MARKET_STATUS_OPEN == marketstatus){
		printf("[T] *********** Welcome to our Market! ***********\n");
	}else{
		printf("[TE] Market is not yeah open!\n");
	}

	return marketstatus;
}



int TradeClient::GetOpenMarketOrderConf(const int CommodityID,  OpenMarketOrderConf &pConf)
{
	int ret = m_Interface->getOpenMarketOrderConf(CommodityID,pConf);
	if(RET_SUCCEED != ret)
	{
		printf("[TE] ACC[%s] GetOpenMarketOrderConf fail! ret = %d\n",LoginAcc.c_str(), ret);
		return ret;
	}
	printf("*********** OpenMarketOrderConf ************\n");
	printf("CommodityID: %d, MinQuantity: %6.3f, MaxQuantity: %6.3f, MinTotalWeight: %6.3f, MaxTotalWeight: %6.3f\n",pConf.CommodityID, pConf.MinQuantity, pConf.MaxQuantity, pConf.MinTotalWeight, pConf.MaxTotalWeight);
	printf("DefaultTradeRange: %6.3f, MinTradeRange: %6.3f, MaxTradeRange: %6.3f\n",pConf.DefaultTradeRange, pConf.MinTradeRange, pConf.MaxTradeRange);
	printf("AgreeUnit: %6.3f, CommodityMode: %s, WeightStep: %6.4f, WeightRadio: %6.3f\n",pConf.AgreeUnit, pConf.CommodityMode==NEW_COMMODITY?"新商品":"旧商品", pConf.WeightStep, pConf.WeightRadio);
	return ret;
}

int TradeClient::GetCloseMarketOrderConf(const int CommodityID,  CloseMarketOrderConf &pConf)
{
	int ret = m_Interface->getCloseMarketOrderConf(CommodityID,pConf);
	if(0 != ret)
	{
		printf("[TE] ACC[%s] GetCloseMarketOrderConf fail! ret = %d\n",LoginAcc.c_str(), ret);
		return ret;
	}
	printf("*********** CloseMarketOrderConf ************\n");
	printf("CommodityID: %d, MinQuantity: %6.3f, MaxQuantity: %6.3f, MinTotalWeight: %6.3f, MaxTotalWeight: %6.3f\n",pConf.CommodityID, pConf.MinQuantity, pConf.MaxQuantity, pConf.MinTotalWeight, pConf.MaxTotalWeight);
	printf("DefaultTradeRange: %6.3f, MinTradeRange: %6.3f, MaxTradeRange: %6.3f\n",pConf.DefaultTradeRange, pConf.MinTradeRange, pConf.MaxTradeRange);
	printf("AgreeUnit: %6.3f, CommodityMode: %s, WeightStep: %6.4f, WeightRadio: %6.3f\n",pConf.AgreeUnit, pConf.CommodityMode==NEW_COMMODITY?"新商品":"旧商品", pConf.WeightStep, pConf.WeightRadio);
	return ret;
}

int TradeClient::GetOpenLimitOrderConf(const int CommodityID,  OpenLimitOrderConf &pConf)
{
	int ret = m_Interface->getOpenLimitOrderConf(CommodityID,pConf);
	if(0 != ret)
	{
		printf("[TE] ACC[%s] GetOpenLimitOrderConf fail! ret = %d\n",LoginAcc.c_str(), ret);
		return ret;
	}

	printf("*********** OpenLimitOrderConf ************\n");
	printf("CommodityID: %d, Min: %6.3f, Max: %6.3f, MinTotalWeight: %6.3f, MaxTotalWeight: %6.3f\n",pConf.CommodityID, pConf.MinQuantity, pConf.MaxQuantity,pConf.MinTotalWeight, pConf.MaxTotalWeight);
	printf("LimitSpread: %6.3f, FixSpread: %6.3f, TPSpread: %6.3f, SLSpread: %6.3f\n",pConf.LimitSpread,pConf.FixSpread,pConf.TPSpread,pConf.SLSpread);
	printf("MinPriceUnit: %6.3f, AgreeUnit: %6.3f\n, CommodityMode: %s, WeightStep: %6.4f, WeightRadio: %6.3f\n",pConf.MinPriceUnit,pConf.AgreeUnit, pConf.CommodityMode==NEW_COMMODITY?"新商品":"旧商品", pConf.WeightStep, pConf.WeightRadio);

	return ret;
} 

int TradeClient::GetCloseLimitOrderConf(const int CommodityID,  LimitClosePositionConf &pConf)
{
	int ret = m_Interface->getLimitClosePositionConf(CommodityID,pConf);
	if(0 != ret)
	{
		printf("[TE] ACC[%s] GetCloseLimitOrderConf fail! ret = %d\n",LoginAcc.c_str(), ret);
		return ret;
	}

	printf("*********** LimitClosePositionConf ************\n");
	printf("CommodityID: %d, TPSpread: %6.3f, SLSpread: %6.3f\n",pConf.CommodityID, pConf.TPSpread, pConf.SLSpread);
	printf("FixedSpread: %6.3f, MinPriceUnit: %6.3f, AgreeUnit: %6.3f\n",pConf.FixedSpread,pConf.MinPriceUnit,pConf.AgreeUnit);
	printf("CommodityMode: %s, WeightRadio: %6.3f\n",pConf.CommodityMode==NEW_COMMODITY?"新商品":"旧商品", pConf.WeightRadio);
	return ret;
}

int TradeClient::GetOpenDeliveryOrderConf(const int CommodityID,const double TotalWeight, const double HoldPrice, OpenDeliveryOderConf &pConf)
{
	int ret = m_Interface->getOpenDeliveryOrderConf(CommodityID, TotalWeight, HoldPrice, pConf);
	if(0 != ret)
	{
		printf("[TE] ACC[%s] GetOpenDeliveryOrderConf fail! ret = %d\n",LoginAcc.c_str(), ret);
		return ret;
	}
	printf("*********** OpenDeliveryOrderConf ************\n");
	printf("CommodityID: %d, CanDeliver:%s, AgreeUnit:%6.3f\n",pConf.CommodityID, pConf.CanDeliver==1?"能":"不能", pConf.AgreeUnit);
	printf("DeliveryCharge:%6.3f,DeliCommCnt:%d\n", pConf.DeliveryCharge, pConf.DeliCommCnt);
	for(int i = 0; i< pConf.DeliCommCnt; i++)
	{
		printf("DeliveryCommodityID:%d, DeliveryCommodityName:%s, ClassID:%d\n",pConf.DeliveryCommodity[i].CommodityID, pConf.DeliveryCommodity[i].CommodityName, pConf.DeliveryCommodity[i].TradecommodityClassID);
		printf("DeliveryRadio:%6.3f, DeliveryCommision:%6.3f\n",pConf.DeliveryCommodity[i].DeliveryRadio, pConf.DeliveryCommodity[i].DeliveryRadio*pConf.DeliveryCharge);
	}
	return ret;
}

int TradeClient::QueryCommodityQuotebyID(const int CommodityID, RealTimeQuote &quote)
{
	int ret = m_Interface->getCommodityQuote(CommodityID, quote);

	if(MSG_STATUS_UPDATED != ret)
	{
		printf("[TE] GetCommodityQuote Fail! ret: %d\n",ret);
		return ret;
	}

	char mDate[30+1];
	TimeToStr(quote.QuoteTime,mDate,sizeof(mDate));
	printf("OneQUOTE: %d,%f,%f,%f,%f,%s\n",quote.CommodityID,quote.BuyPrice,quote.SellPrice,quote.HighPrice,quote.LowPrice,mDate);

	return ret;
}


int TradeClient::QueryHoldPositionByID(const long long nHoldPositionID, HoldPositionInfo& pHoldPosition)
{

	int ret = m_Interface->getHoldPositionByID(nHoldPositionID,pHoldPosition);
	if(MSG_STATUS_UPDATED != ret)
	{
		printf("[TE] GetHoldPositionByID Fail! ret: %d\n",ret);
		return ret;
	}

	printf("[T] HoldPositionID:%lld, CommodityID:%d\n", pHoldPosition.HoldPositionID,pHoldPosition.CommodityID);
	printf("\t CommodityName:%s,  openType:%d,  openDirect:%s\n",pHoldPosition.CommodityName,pHoldPosition.OpenType,pHoldPosition.OpenDirector==1?"1买入":"2卖出");
	printf("\t Quantity:%d, OpenPrice:%6.3f, HoldPositionPrice:%6.3f\n",pHoldPosition.Quantity,pHoldPosition.OpenPrice,pHoldPosition.HoldPositionPrice);
	printf("\t SLLimitOrderID:%lld, SLPrice:%6.3f, TPLimitOrderID:%lld, TPPrice:%6.3f\n",pHoldPosition.SLLimitOrderID,pHoldPosition.SLPrice, pHoldPosition.TPLimitOrderID,pHoldPosition.TPPrice);
	printf("\t OpenProfit:%6.3f, CommissionAmount:%6.3f\n", pHoldPosition.OpenProfit, pHoldPosition.CommissionAmount);
	printf("\t AgreeMargin:%6.3f, Freezemargin:%6.3f, OverdueFindFund:%6.3f\n", pHoldPosition.AgreeMargin, pHoldPosition.Freezemargin, pHoldPosition.OverdueFindFund);
	printf("------------------------------------\n");

	return ret;
} 

int TradeClient::QueryLimitOrderByID(const long long nLimitOrderID,  LimitOrderInfo& pLimitOrder)
{
	int ret = m_Interface->getLimitOrderByID(nLimitOrderID,pLimitOrder);
	if(MSG_STATUS_UPDATED != ret)
	{
		printf("[TE] GetLimitOrderByID Fail! ret: %d\n",ret);
		return ret;
	}

	char createdate[30+1];
	char updatedate[30+1];
	TimeToStr(pLimitOrder.CreateDate,createdate,sizeof(createdate));
	TimeToStr(pLimitOrder.UpdateDate,updatedate,sizeof(updatedate));

	printf("[T] LimitOrderID:%lld, CommodityID:%d\n", pLimitOrder.LimitOrderID,pLimitOrder.CommodityID);
	printf("\t CommodityName:%s,  LimitType:%d, OrderType:%d, openDirect:%s\n",pLimitOrder.CommodityName,pLimitOrder.LimitType,pLimitOrder.OrderType,pLimitOrder.OpenDirector==1?"1买入":"2卖出");
	printf("\t Quantity:%d, OrderPrice:%6.3f, HoldPositionPrice:%s\n",pLimitOrder.OpenQuantity,pLimitOrder.OrderPrice,createdate);
	printf("\t SLPrice:%6.3f, TPPrice:%6.3f, UpdateDate:%s\n",pLimitOrder.SLPrice, pLimitOrder.TPPrice, updatedate);
	printf("\t ExpireType:%lld, FreeszMargin:%6.3f\n", pLimitOrder.ExpireType, pLimitOrder.FreeszMargin);
	printf("------------------------------------\n");

	return ret;

}
int TradeClient::QueryClosePositionByID(const long long nClosePositionID, ClosePositionInfo& pClosePosition)
{
	int ret = m_Interface->getClosePositionByID(nClosePositionID,pClosePosition);
	if(MSG_STATUS_UPDATED != ret)
	{
		printf("[TE] GetClosePositionByID Fail! ret: %d\n",ret);
		return ret;
	}

	char opendate[30+1];
	char closedate[30+1];
	TimeToStr(pClosePosition.OpenDate,opendate,sizeof(opendate));
	TimeToStr(pClosePosition.CloseDate,closedate,sizeof(closedate));

	printf("[T] ClosePositionID:%lld, CommodityID:%d, MemberID:%d\n", pClosePosition.ClosePositionID,pClosePosition.CommodityID, pClosePosition.MemberID);
	printf("\t CommodityName:%s,  OpenType:%d, CloseType:%d, CloseDirector:%s\n",pClosePosition.CommodityName,pClosePosition.OpenType,pClosePosition.CloseType,pClosePosition.CloseDirector==1?"1买入":"2卖出");
	printf("\t Quantity:%d, OpenPrice:%6.3f, HoldPrice:%6.3f, OpenPositionID:%lld\n",pClosePosition.Quantity,pClosePosition.OpenPrice,pClosePosition.HoldPrice,pClosePosition.OpenPositionID);
	printf("\t ClosePrice:%6.3f, CommissionAmount:%6.3f, OpenDate:%s, CloseDate:%s\n",pClosePosition.ClosePrice, pClosePosition.CommissionAmount, opendate, closedate);
	printf("------------------------------------\n");
	
	return ret;
}

int TradeClient::QueryHoldPositionTotalByCommodityID(const int nCommodityID, const int nOpenDirector, HoldPositionTotalInfo& pTotal)
{
	int ret = m_Interface->getHoldPositionTotalByCommodityID(nCommodityID,nOpenDirector,pTotal);
	if(MSG_STATUS_UPDATED != ret)
	{
		printf("[TE] GetHoldPositionTotalByCommodityID Fail! ret: %d\n",ret);
		return ret;
	}

	printf("[T] CommodityID:%d, CommodityName:%s\n", pTotal.CommodityID,pTotal.CommodityName);
	printf("\t OpenDirector:%s,Quantity:%d, \n",pTotal.OpenDirector==1?"1买入":"2卖出", pTotal.Quantity);
	printf("\t OpenPriceTotal:%6.3f, AvgOpenPrice:%6.3f, HoldPriceTotal:%6.3f, AvgHoldPrice:%6.3f\n",pTotal.OpenPriceTotal,pTotal.AvgOpenPrice,pTotal.HoldPriceTotal,pTotal.AvgHoldPrice );
	printf("\t ClosePrice:%6.3f, OpenProfit:%6.3f\n",pTotal.ClosePrice, pTotal.OpenProfit);
	printf("------------------------------------\n");

	return ret;
}

int TradeClient::QueryHoldPositionTotalByDirector(const int nOpenDirector, vector<HoldPositionTotalInfo> &vTotal)
{
	m_vecHoldPositionTotal.clear();

	int ret = m_Interface->getHoldPositionTotal(m_vecHoldPositionTotal);

	if(MSG_STATUS_UPDATED != ret )
	{
		printf("[TE] GetHoldPositionTotalByDirector Fail! ret: %d\n",ret);
		return ret;
	}

    std::vector<HoldPositionTotalInfo>::iterator it;
	for(it=m_vecHoldPositionTotal.begin();it!=m_vecHoldPositionTotal.end();it++)
	{
		if(nOpenDirector == it->OpenDirector)
		{
			vTotal.push_back(*it);
		}
	}

	printf("============== HoldPositionTotal =======================\n");
	for(it=vTotal.begin();it!=vTotal.end();it++)
	{

		printf("[T] CommodityID:%d, CommodityName:%s\n", it->CommodityID,it->CommodityName);
		printf("\t OpenDirector:%s,Quantity:%d, \n",it->OpenDirector==1?"1买入":"2卖出", it->Quantity);
		printf("\t OpenPriceTotal:%6.3f, AvgOpenPrice:%6.3f, HoldPriceTotal:%6.3f, AvgHoldPrice:%6.3f\n",it->OpenPriceTotal,it->AvgOpenPrice,it->HoldPriceTotal,it->AvgHoldPrice );
		printf("\t ClosePrice:%6.3f, OpenProfit:%6.3f\n",it->ClosePrice, it->OpenProfit);
		printf("------------------------------------\n");
	}

	return ret;
}

int TradeClient::RequestQuote()
{
	m_vecCommodity.clear();
	int ret = m_Interface->getCommodity(m_vecCommodity);

	if(MSG_STATUS_UPDATED != ret )
	{
		printf("[TE] requestQuote Fail! ret: %d\n",ret);
		return ret;
	}

	std::vector<CommodityInfo>::iterator vit;
	
	printf("============== Commoditys =======================\n");
	for(vit=m_vecCommodity.begin();vit!=m_vecCommodity.end();vit++)
	{
		char date[30+1];
		TimeToStr(vit->QuoteTime,date,sizeof(date));
		printf("[T] OneQUOTE: %d,%s,%f,%f,%f,%f,%s,%f,%lld,%lld,%d,%s,%d,%s,%s,%d\n",
				vit->CommodityID,vit->CommodityName,vit->BuyPrice,vit->SellPrice,vit->HighPrice,
				vit->LowPrice,date,vit->WeightStep,vit->CommodityRight,vit->TradeRight,
				vit->CommodityClass,vit->CommodityClassName,vit->CommodityGroupID,vit->CommodityGroupName,
				vit->CommodityMode==1?"新商品":"旧商品",vit->IsDisplay);

	}

	return ret;
}


int TradeClient::RequestHoldPositionTotal()
{
	m_vecHoldPositionTotal.clear();

	int ret = m_Interface->getHoldPositionTotal(m_vecHoldPositionTotal);
	
	if(MSG_STATUS_UPDATED != ret )
	{
		printf("[TE] requestHoldPositionTotal Fail! ret: %d\n",ret);
		return ret;
	}

    std::vector<HoldPositionTotalInfo>::iterator it;
	printf("============== HoldPositionTotal =======================\n");
	for(it=m_vecHoldPositionTotal.begin();it!=m_vecHoldPositionTotal.end();it++)
	{
		printf("[T] CommodityID:%d, CommodityName:%s\n", it->CommodityID,it->CommodityName);
		printf("\t OpenDirector:%s,Quantity:%d, TotalWeight:%6.3f \n",it->OpenDirector==1?"1买入":"2卖出", it->Quantity, it->TotalWeight);
		printf("\t OpenPriceTotal:%6.3f, AvgOpenPrice:%6.3f, HoldPriceTotal:%6.3f, AvgHoldPrice:%6.3f\n",it->OpenPriceTotal,it->AvgOpenPrice,it->HoldPriceTotal,it->AvgHoldPrice );
		printf("\t ClosePrice:%6.3f, OpenProfit:%6.3f\n",it->ClosePrice, it->OpenProfit);
		printf("------------------------------------\n");
	}

	return ret;
}


int TradeClient::RequestHoldPosition()
{
	m_vecHoldPosition.clear();

    int ret = m_Interface->getPositionOrder(m_vecHoldPosition);

	if(MSG_STATUS_UPDATED != ret )
	{
		printf("[TE] requestHoldPosition Fail! ret: %d\n",ret);
		return ret;
	}

    std::vector<HoldPositionInfo>::iterator it;
	printf("============== HoldPositions =======================\n");
	for(it=m_vecHoldPosition.begin();it!=m_vecHoldPosition.end();it++)
	{
		char opendate[30+1];
		TimeToStr(it->OpenDate,opendate,sizeof(opendate));

		printf("[T] HoldPositionID:%lld, CommodityID:%d\n", it->HoldPositionID,it->CommodityID);
		printf("\t CommodityName:%s,  openType:%d,  openDirect:%s\n",it->CommodityName,it->OpenType,it->OpenDirector==1?"1买入":"2卖出");
		printf("\t Quantity:%d, Weight:%f, OpenPrice:%6.3f, HoldPositionPrice:%6.3f\n",it->Quantity,it->TotalWeight, it->OpenPrice,it->HoldPositionPrice);
		printf("\t ClosePrice:%6.3f, SLLimitOrderID:%lld, SLPrice:%6.3f, TPLimitOrderID:%lld, TPPrice:%6.3f\n",it->ClosePrice, it->SLLimitOrderID,it->SLPrice, it->TPLimitOrderID,it->TPPrice);
		printf("\t OpenProfit:%6.3f, CommissionAmount:%6.3f, OpenDate:%s\n", it->OpenProfit, it->CommissionAmount, opendate);
		printf("\t AgreeMargin:%6.3f, Freezemargin:%6.3f, OverdueFindFund:%6.3f\n", it->AgreeMargin, it->Freezemargin, it->OverdueFindFund);
		printf("------------------------------------\n");
	}

	return ret;
}


int TradeClient::RequestLimitOrders()
{
	m_vecLimitOrder.clear();

	int ret = m_Interface->getLimitOrder(m_vecLimitOrder);

	if(MSG_STATUS_UPDATED != ret )
	{
		printf("[TE] requestLimitOrders Fail! ret: %d\n",ret);
		return ret;
	}

    std::vector<LimitOrderInfo>::iterator it;
	printf("============== LimitOrders =======================\n");
	for(it=m_vecLimitOrder.begin();it!=m_vecLimitOrder.end();it++)
	{
		char createdate[30+1];
		char updatedate[30+1];
		TimeToStr(it->CreateDate,createdate,sizeof(createdate));
		TimeToStr(it->UpdateDate,updatedate,sizeof(updatedate));

		printf("[T] LimitOrderID:%lld, CommodityID:%d\n", it->LimitOrderID,it->CommodityID);
		printf("\t CommodityName:%s,  LimitType:%d, OrderType:%d, openDirect:%s\n",it->CommodityName,it->LimitType,it->OrderType,it->OpenDirector==1?"1买入":"2卖出");
		printf("\t Quantity:%d, Weight:%6.3f, OrderPrice:%6.3f, CreateDate:%s\n",it->OpenQuantity, it->TotalWeight,it->OrderPrice,createdate);
		printf("\t SLPrice:%6.3f, TPPrice:%6.3f, UpdateDate:%s\n",it->SLPrice, it->TPPrice, updatedate);
		printf("\t ExpireType:%lld, FreeszMargin:%f, DealStatus:%d\n", it->ExpireType, it->FreeszMargin, it->DealStatus);
		printf("------------------------------------\n");
	}

	return ret;
}


int TradeClient::RequestCloseOrder()
{
	m_vecClosePostions.clear();

	int ret = m_Interface->getClosePosition(m_vecClosePostions);

	if(MSG_STATUS_UPDATED != ret )
	{
		printf("[TE] requestCloseOrder Fail! ret: %d\n",ret);
		return ret;
	}

    std::vector<ClosePositionInfo>::iterator it;
	printf("============== ClosePositions =======================\n");
	for(it=m_vecClosePostions.begin();it!=m_vecClosePostions.end();it++)
	{
		char opendate[30+1];
		char closedate[30+1];
		TimeToStr(it->OpenDate,opendate,sizeof(opendate));
		TimeToStr(it->CloseDate,closedate,sizeof(closedate));

		printf("[T] ClosePositionID:%lld, CommodityID:%d, MemberID:%d\n", it->ClosePositionID,it->CommodityID, it->MemberID);
		printf("\t CommodityName:%s,  OpenType:%d, CloseType:%d, CloseDirector:%s\n",it->CommodityName,it->OpenType,it->CloseType,it->CloseDirector==1?"1买入":"2卖出");
		printf("\t Quantity:%d, Weight:%6.3f, OpenPrice:%6.3f, HoldPrice:%6.3f, OpenPositionID:%lld\n",it->Quantity, it->TotalWeight, it->OpenPrice,it->HoldPrice,it->OpenPositionID);
		printf("\t ClosePrice:%6.3f, CommissionAmount:%6.3f, OpenDate:%s, CloseDate:%s\n",it->ClosePrice, it->CommissionAmount, opendate, closedate);
		printf("------------------------------------\n");
	}

	return ret;
}

int TradeClient::RequestDeliveryOrder()
{
	m_vecDeliveryOrder.clear();
	int ret = m_Interface->getDeliveryOrder(m_vecDeliveryOrder);
	if(MSG_STATUS_UPDATED != ret )
	{
		printf("[TE] requestDeliveryOrder Fail! ret: %d\n", ret);
		return ret;
	}

	int counter = 0;
	std::vector<DeliveryOrderInfo>::iterator it;
	printf("============== DeliveryOrders =======================\n");
	for(it=m_vecDeliveryOrder.begin(); it!= m_vecDeliveryOrder.end(); it++)
	{
		counter++;
		char deliverydate[30+1];
		char applydate[30+1];
		char deliverydeadline[30+1];
		TimeToStr(it->DeliveryDate,deliverydate,sizeof(deliverydate));
		TimeToStr(it->ApplyDate,applydate,sizeof(applydate));
		TimeToStr(it->DeliveryDeadline,deliverydeadline,sizeof(deliverydeadline));

		printf("[T] CNT[%d], DeliveryOrderID:%lld, CommodityID:%d, HoldPositionID:%lld\n", counter, it->DeliveryOrderID, it->CommodityID, it->HoldpositionID);
		printf("\t DeliveryCommodityName:%s, DeliveryPrice:%6.3f, Quantity:%d, TotalWeight:%6.3f, Amount:%6.3f, Commission:%6.3f\n",it->DeliveryCommodityName,it->DeliveryPrice,it->Quantity,it->TotalWeight,it->Amount, it->Commission);
		printf("\t ApplyDate:%s, DeliveryDate:%s, DeliveryDeadline:%s, DeliveryStatus:%d,DeliveryType:%s\n",applydate,deliverydate,deliverydeadline,it->DeliveryStatus,it->DeliveryType==0?"买方交割":"未知类型");
		printf("------------------------------------\n");
	}
	return ret;
}

int TradeClient::OnOpenMarketOrder(OpenMarketOrderParam &Order)
{
	m_bMutex.Lock();
	int ret = m_Interface->doOpenMarketOrder(Order);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnOpenMarketOrder fail! ret: %d\n",ret);
		return ret;
	}
	return ret;
}

int TradeClient::OnCloseMarketOrder(CloseMarketOrderParam &Order)
{ 	
	m_bMutex.Lock();
	int ret = m_Interface->doCloseMarketOrder(Order);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnCloseMarketOrder fail! ret: %d\n",ret);
		return ret;
	}
	return ret; 
}

int TradeClient::OnOpenLimitOrder(OpenLimitOrderParam &Order)
{ 	
	m_bMutex.Lock();
	int ret = m_Interface->doOpenLimitOrder(Order);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnOpenLimitOrder fail! ret: %d\n",ret);
		return ret;
	}
	return ret; 
}

int TradeClient::OnLimitClosePosition(CloseLimitOrderParam &Order)
{ 	
	m_bMutex.Lock();
	int ret = m_Interface->doLimitClosePosition(Order);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnLimitClosePosition fail! ret: %d\n",ret);
		return ret;
	}
	return ret;
}

int TradeClient::OnCustmTradeReportClosePosition(ReportQueryParam &param)
{
	m_bMutex.Lock();
	int ret = m_Interface->doCustmTradeReportClosePositionQuery(param);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnCustmTradeReportClosePosition fail! ret: %d\n",ret);
		return ret;
	}
	return ret;
}

int TradeClient::OnCustmTradeReportHoldPosition(ReportQueryParam &param)
{
	m_bMutex.Lock();
	int ret = m_Interface->doCustmTradeReportHoldPositionQuery(param);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnCustmTradeReportHoldPosition fail! ret: %d\n",ret);
		return ret;
	}
	return ret;
}

int TradeClient::OnCustmTradeReportLimitOrder(ReportQueryParam &param)
{
	m_bMutex.Lock();
	int ret = m_Interface->doCustmTradeReportLimitOrderQuery(param);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnCustmTradeReportLimitOrder fail! ret: %d\n",ret);
		return ret;
	}
	return ret;
}

int TradeClient::OnCustomerSelfFundFlowQuery(FundFlowQueryParam &param)
{
	m_bMutex.Lock();
	int ret = m_Interface->doCustomerSelfFundFlowQuery(param);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnCustomerSelfFundFlowQuery fail! ret: %d\n",ret);
		return ret;
	}
	return ret;
}

int TradeClient::OnCloseMarketOrderMany(CloseMarketOrderManyParam &Order)
{	
	m_bMutex.Lock();
	int ret = m_Interface->doCloseMarketOrderMany(Order);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnCloseMarketOrderMany fail! ret: %d\n",ret);
		return ret;
	}
	return ret; 
}

int TradeClient::OnLimitRevoke(LimitRevokeParam &Order)
{	
	m_bMutex.Lock();
	int ret = m_Interface->doLimitRevoke(Order);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnLimitRevoke fail! ret: %d\n",ret);
		return ret;
	}
	return ret; 
}

int TradeClient::OnMoneyInOut(MoneyInOutParam &Order)
{	
	int time = 5;
	while(MSG_STATUS_UPDATED != m_MoneyStatus){
		sleep(1);
		if(0 == (--time)){
			printf("[TE] Please QueryMoney First!\n");
			return -1;
		}
	}
	m_bMutex.Lock();
	int ret = m_Interface->doMoneyInOut(Order);
	m_bMutex.Unlock();

	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnMoneyInOut fail! ret: %d\n",ret);
	}
	m_MoneyStatus = MSG_STATUS_NO_UPDATE;
	return ret; 
}

int TradeClient::OnMoneyQuery()
{
	m_bMutex.Lock();
	int ret = m_Interface->doMoneyQuery();
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnMoneyQuery fail! ret: %d\n",ret);
	}

	return ret;
}

int TradeClient::OnOpenDeliveryOrder(OpenDeliveryOrderParam &Order)
{       
	m_bMutex.Lock();
	int ret = m_Interface->doOpenDeliveryOrder(Order);
	m_bMutex.Unlock();
	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnOpenDeliveryOrder fail! ret: %d\n",ret);
		return ret;
	}
	return ret;
}

int TradeClient::OnSignResultNotifyQuery(SignResultNotifyQueryParam &mParam)
{
	m_bMutex.Lock();
	int ret = m_Interface->doSignResultNotifyQuery(mParam);
	m_bMutex.Unlock();

	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnSignResultNotifyQuery fail! ret: %d\n", ret);
	}

	return ret;
}

int TradeClient::OnPayForward(PayForwardParam mParam)
{
	m_bMutex.Lock();
	int ret = m_Interface->doPayForward(mParam);
	m_bMutex.Unlock();

	if(Order_Ret_Success != ret)
	{
		printf("[TE] OnPayForward fail! ret: %d\n", ret);
	}

	return ret;
}

int TradeClient::GetCurrStatus(){
	return gStatus;
}

int TradeClient::StartHeartBeat()
{
	int ret = 0;
	m_bMutex.Lock();
	if(m_Interface != NULL)
		ret = m_Interface->doHeartBeat();
	m_bMutex.Unlock();
	if(RET_FATAL_DROPDOWN == ret)
	{
		printf("[TE] ACC[%s] HeartBeat fail!\n",LoginAcc.c_str());
		gStatus = -40; 
	}
	return ret;
}

