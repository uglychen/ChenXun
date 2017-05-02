#pragma once

#include <time.h>
#include <map>

#include "TradeInterface.h"
//#include "CChineseCode.h"
#include "AppLog.h"
#include "QuoteServiceInit.h"

#include "TQuoteEvent.h"
#include "TLoginEvent.h"
#include "TOpenMarketOrderEvent.h"
#include "TCloseMarketOrderEvent.h"
#include "TOpenLimitOrderEvent.h"
#include "TLimitClosePositionEvent.h"
#include "TCustmTradeReportClosePositionQueryEvent.h"
#include "TCustmTradeReportHoldPositionQueryEvent.h"
#include "TCustmTradeReportLimitOrderQueryEvent.h"
#include "TCustomerSelfFundFlowQueryEvent.h"
#include "TCustomerAllowLoginEvent.h"
#include "TRevokeLimitOrderEvent.h"
#include "TSysBulletin.h"
#include "TSysBulletinLimitCloseEvent.h"
#include "TSysBulletinLiquidationEvent.h"
#include "TDisConnectEvent.h"
#include "TMoneyInOutEvent.h"
#include "TMoneyQueryEvent.h"
#include "TPwdModifyEvent.h"
#include "TSignResultNotifyQueryEvent.h"
#include "TPayForwardEvent.h"
#include "TOpenDeliveryOrderEvent.h"

#ifdef _WIN32
#pragma comment(lib, "TradeInterfaceSDK.lib")
#endif

#define Event_Ret_Success 99999
#define Event_Ret_DROPUSER 4006
#define Event_Ret_LOGACCERR 4001
#define Order_Ret_Success 0

#define TS_1970_TO_1900  2208988800

class TradeClient: public Client
{
public:
	TradeClient();
	~TradeClient();

public:
	int Init();
	int Release();

	int Login(std::string ploginacc, std::string ppwd);
	int LoginPwdUpd(std::string oldpwd, std::string newpwd, int markettype);
	int FundPwdUpd(std::string oldpwd, std::string newpwd);
	int Logout();

	int OnCustomerAllowLogin(AllowLoginParam mParam);
	int GetAccountInfo();  // 账户信息
	//int GetMoneyInfo();	// 银行资金信息
	//int GetCustomerBankInfo();	// 客户银行信息

	int GetMarketStatus();  // 开休市状态
	int GetOpenMarketOrderConf(const int CommodityID,  OpenMarketOrderConf &pConf);
	int GetCloseMarketOrderConf(const int CommodityID, CloseMarketOrderConf &pConf);
	int GetOpenLimitOrderConf(const int CommodityID,   OpenLimitOrderConf &pConf);
	int GetCloseLimitOrderConf(const int CommodityID,  LimitClosePositionConf &pConf);
	int GetOpenDeliveryOrderConf(const int CommodityID,const double TotalWeight, const double HoldPrice, OpenDeliveryOderConf &pConf);

	int QueryCommodityQuotebyID(const int CommodityID, RealTimeQuote &quote);  // 单商品行情
	int QueryHoldPositionByID(const long long nHoldPositionID, HoldPositionInfo& pHoldPosition); // 根据持仓单ID获取其详情
	int QueryLimitOrderByID(const long long nLimitOrderID,  LimitOrderInfo& pLimitOrder); // 根据限价单ID获取其详情
	int QueryClosePositionByID(const long long nClosePositionID, ClosePositionInfo& pClosePosition); // 根据平仓单ID获取其详情
	int QueryHoldPositionTotalByCommodityID(const int nCommodityID, const int nOpenDirector, HoldPositionTotalInfo& pTotal); // 根据持仓汇总单ID和方向，获取其详情
	int QueryHoldPositionTotalByDirector(const int nOpenDirector, vector<HoldPositionTotalInfo> &vTotal); // 根据持仓汇总单方向，获取各单详情


	int RequestQuote();  // 商品行情
	int RequestHoldPosition(); // 持仓单
	int RequestLimitOrders(); // 限价单
	int RequestCloseOrder();  // 平仓单
	int RequestHoldPositionTotal(); // 持仓汇总
	int RequestDeliveryOrder(); // 交割单

	int OnOpenMarketOrder(OpenMarketOrderParam &Order);  // 市价单建仓
	int OnCloseMarketOrder(CloseMarketOrderParam &Order);  // 市价单平仓
	int OnOpenLimitOrder(OpenLimitOrderParam &Order);    // 限价单建仓
	int OnLimitClosePosition(CloseLimitOrderParam &Order);     // 限价单平仓
	int OnCustmTradeReportClosePosition(ReportQueryParam &param);
	int OnCustmTradeReportHoldPosition(ReportQueryParam &param);
	int OnCustmTradeReportLimitOrder(ReportQueryParam &param);
	int OnCustomerSelfFundFlowQuery(FundFlowQueryParam &param);
	int OnCloseMarketOrderMany(CloseMarketOrderManyParam &Order);   // 批量平仓
	int OnLimitRevoke(LimitRevokeParam &Order);     //  撤销订单
	int OnMoneyInOut(MoneyInOutParam &Order);	// 出入金
	int OnMoneyQuery();	// 查询银行资金
	int OnOpenDeliveryOrder(OpenDeliveryOrderParam &Order); // 交割单
	int OnSignResultNotifyQuery(SignResultNotifyQueryParam &mParam); //签约结果通知
	int OnPayForward(PayForwardParam mParam); //支付推进

	int GetCurrStatus();
	//int Signal_task();
	int StartHeartBeat();
	int ReInit();
private:
	int InitNetwork();
	//int StartHeartBeat();
	//int StopHeartBeat();

	void procTradeEvent(const TradeEvent *pEvent);

	int TimeToStr(const long long second, char* dest, int len);

	//int register_my_signal();
private:
	TradeInterface				  *m_Interface;
//	CChineseCode                  *m_cc;

	vector<CommodityInfo>		  m_vecCommodity;
	vector<HoldPositionInfo>      m_vecHoldPosition;
    vector<HoldPositionTotalInfo> m_vecHoldPositionTotal;
    vector<LimitOrderInfo>        m_vecLimitOrder;
    vector<ClosePositionInfo>     m_vecClosePostions;
	vector<DeliveryOrderInfo>     m_vecDeliveryOrder;


	BourseMoneyInfo		m_BourseMoneyInfo;
	int					m_MoneyStatus;
	int					gStatus; ///< -99 new| 0 init| 1 login| -1 Disconnect| -2 logout| -99 NULL
	
	JMutex				m_bMutex;
	time_t              last;
public:
	string              LoginAcc;

};

std::string getime();
int register_my_signal();
int Signal_task();
