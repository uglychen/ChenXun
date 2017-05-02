#ifndef __TRADE_INTERFACE_H__
#define __TRADE_INTERFACE_H__


#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>
#include "TradeCommon.h"
#include "jthread.h"
#include "jmutex.h"
#include "Client.h"
#include "TradeStruct.h"
#include "TradeEventCode.h"
#include "TradeDefine.h"
#include "TSysBulletin.h"
#include "common/tradeclient.h"

using namespace std;
using namespace TradeAPI;

namespace TradeAPI
{
namespace business
{
	class HeartBeatEvent;
	class LoginEvent;
	class LoginPwdUpdEvent;
	class FundPwdUpdEvent;
	class TokenLoginEvent;
	class CustomerAllowLoginEvent;
	class UserCommodityInfoEvent;

	class MarketStatusQueryEvent;
	class UserCommodityFeeGroupEvent;
	class ClientConfigQueryEvent;
	class DeliveryCommodityQueryEvent;

	class OpenPositionEvent;
	class ClosePositionEvent;
	class LimitOpenPositionEvent;
	class LimitClosePositionEvent;
	class LimitRevokeEvent;
	class CustomerCloseManyEvent;
	class OpenDeliveryEvent;

	class AccountQueryEvent;
	class HoldPositionOrderQueryEvent;
	class LimitOrderQueryEvent;
	class ClosePositionQueryEvent;
	class DeliveryOrderQueryEvent;
    class CustmTradeReportHoldPositionQueryEvent;
	class CustmTradeReportClosePositionQueryEvent;
	class CustmTradeReportLimitOrderQueryEvent;
	class CustomerSelfFundFlowQueryEvent;
	class SysBulletinEvent;

	class MoneyInOutEvent;
	class MoneyQueryEvent;
	class SignResultNotifyQueryEvent;
	class PayForwardEvent;
	class CustomerBankInfoEvent;
	class DisconnectEvent;
	class DepositeQueryEvent;

	class TradeManager;
	class BusinessMessage;
	class QuoteManager;

}
}

///交易接口类
class SDKDLL_API TradeInterface:  public JThread, public TradeAPI::common::TradeClient
{
public:
	TradeInterface();
	~TradeInterface();

public:

///****************************************
/// @brief 用于初始化
/// @param  strIP		:	IP地址
/// @param  nPort		: 端口号
/// @param  strLogPath		:	日志路径
/// @return 返回处理结果状态 
/// - 0 : 初始化成功
/// - !0 : 失败
///****************************************
	int  Init(string strIP, int nPort, string strLogPath);
	
	
///****************************************
/// @brief  释放资源 
/// @param  
/// @return 返回处理结果状态
///  - 0 : 成功
///  - !0 : 失败 
///****************************************
	int  Release();


///****************************************
/// @brief  将Client类对象注册到接口对象里
/// @param  *p	:	Client对象指针
/// @return 
///****************************************
	void RegisterClient(Client* p);
	
	
///****************************************
/// @brief  设置应用交易码(也即授权码)和校验密码。 
/// @param  strAppCode		:  应用交易码
/// @param  Password		:  校验密码
/// @return   
///*****************************************
	void SetAppMessage(string strAppCode, string Password);

///****************************************
/// @brief  设置会员ID。 
/// @param  vendorid		:  会员ID
/// @return   
///*****************************************
	void setVendorID(string vendorid);

///****************************************
/// @brief  用于登录。 
/// @param  strLoginAccount		:	客户账户名
/// @param  strPwd						:	密码
/// @return 返回处理结果状态
///  -  0 : 成功 
///  - !0 : 失败
///*****************************************
	int  doLogin(string strLoginAccount, string strPwd);
	int  doLogin(string strLoginAccount, string strPwd, string &strSid);

///****************************************
/// @brief  用于修改登陆密码。
/// @param  oldPwd     :   原密码
/// @param  newPwd     :   新密码
/// @param	marketType  :   市场类型 1：贵金属 2：原油
/// @return 返回处理结果状态
///  -  0 : 成功
///  - !0 : 失败
///*****************************************
    int  doLoginPwdUpd(string oldPwd, string newPwd, int marketType);
	int  doLoginPwdUpd(string oldPwd, string newPwd, int marketType, string &strSid);

///****************************************
/// @brief  用于修改登陆密码
/// @param  oldPwd     :   原密码
/// @param  newPwd     :   新密码
/// @return 返回处理结果状态
///  -  0 : 成功
///  - !0 : 失败
///*****************************************
	int  doFundPwdUpd(string oldPwd, string newPwd);
	int  doFundPwdUpd(string oldPwd, string newPwd, string &strSid);

///****************************************
/// @brief  用于登录。 
/// @param  strAccessToken		:	客户接入凭证
/// @param  strClientID			:	客户端ID
/// @return 返回处理结果状态
///  -  0 : 成功 
///  - !0 : 失败
///*****************************************
	int  doTokenLogin(string strAccessToken, string strClientID);
	int  doTokenLogin(string strAccessToken, string strClientID, string &strSid);

///****************************************
/// @brief  用于修改或查询客户是否可以登录 
/// @param  mParam      :   AllowLoginParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败
///*****************************************
    int  doCustomerAllowLogin(AllowLoginParam mParam);
    int  doCustomerAllowLogin(AllowLoginParam mParam, string &strSid);


///****************************************
/// @brief  市价单建仓 
/// @param  mParam		:	OpenMarketOrderParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///*****************************************
	int  doOpenMarketOrder(OpenMarketOrderParam& mParam);
	int  doOpenMarketOrder(OpenMarketOrderParam& mParam, string &strSid);

	
///****************************************
/// @brief 市价单平仓 
/// @param mParam			:	CloseMarketOrderParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///*****************************************	
	int  doCloseMarketOrder(CloseMarketOrderParam& mParam);
	int  doCloseMarketOrder(CloseMarketOrderParam& mParam, string &strSid);


///****************************************
/// @brief  限价单建仓 
/// @param  mParam		:	OpenLimitOrderParam 结构体
/// @return  
///  -  0 : 成功 
///  - !0 : 失败 
///****************************************
	int  doOpenLimitOrder(OpenLimitOrderParam& mParam);
	int  doOpenLimitOrder(OpenLimitOrderParam& mParam, string &strSid);


///****************************************
/// @brief 限价单平仓
/// @param mParam			:	CloseLimitOrderParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///*****************************************
	int  doLimitClosePosition(CloseLimitOrderParam& mParam);
	int  doLimitClosePosition(CloseLimitOrderParam& mParam, string &strSid);


///****************************************
/// @brief 限价单撤销 
/// @param mParam		:	LimitRevokeParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///****************************************
	int  doLimitRevoke(LimitRevokeParam& mParam);
	int  doLimitRevoke(LimitRevokeParam& mParam, string &strSid);


///****************************************
/// @brief 批量平仓 
/// @param mParam				:	CloseMarketOrderManyParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///****************************************
	int  doCloseMarketOrderMany(CloseMarketOrderManyParam& mParam);
	int  doCloseMarketOrderMany(CloseMarketOrderManyParam& mParam, string &strSid);

///****************************************
/// @brief 申请交割 
/// @param mParam				:	OpenDeliveryOrderParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///****************************************
	int  doOpenDeliveryOrder(OpenDeliveryOrderParam& mParam);
	int  doOpenDeliveryOrder(OpenDeliveryOrderParam& mParam, string &strSid);

///****************************************
/// @brief 客户交易报表持仓单查询请求
/// @param mParam           :   ReportQueryParam 结构体
/// @return
///  -  0 : 成功
///  - !0 : 失败
///*****************************************
    int  doCustmTradeReportHoldPositionQuery(ReportQueryParam mParam);
	int  doCustmTradeReportHoldPositionQuery(ReportQueryParam mParam, string &strSid);

///****************************************
/// @brief 客户交易报表平仓单查询请求
/// @param mParam           :   ReportQueryParam 结构体
/// @return
///  -  0 : 成功
///  - !0 : 失败
///*****************************************
    int  doCustmTradeReportClosePositionQuery(ReportQueryParam mParam);
	int  doCustmTradeReportClosePositionQuery(ReportQueryParam mPara, string &strSid);

///****************************************
/// @brief 客户交易报表限价单查询请求
/// @param mParam           :   ReportQueryParam 结构体
/// @return
///  -  0 : 成功
///  - !0 : 失败
///*****************************************
    int  doCustmTradeReportLimitOrderQuery(ReportQueryParam mParam);
	int  doCustmTradeReportLimitOrderQuery(ReportQueryParam mParam, string &strSid);

///****************************************
/// @brief 出入金查询请求
/// @param mParam           :   FundFlowQueryQueryParam 结构体
/// @return
///  -  0 : 成功
///  - !0 : 失败
///*****************************************
	int  doCustomerSelfFundFlowQuery(FundFlowQueryParam mParam);
	int  doCustomerSelfFundFlowQuery(FundFlowQueryParam mParam, string &strSid);


///****************************************
/// @brief 出入金 
/// @param mParam				:	MoneyInOutParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///****************************************
	int	 doMoneyInOut(MoneyInOutParam& mParam);
	int  doMoneyInOut(MoneyInOutParam& mParam, string &strSid);


///****************************************
/// @brief 查询银行资金 
/// @param
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///****************************************
	int  doMoneyQuery();
	int  doMoneyQuery(string &strSid);

///****************************************
/// @brief 签约结果通知
/// @param mParam           :  SignResultNotifyQueryParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///****************************************
	int doSignResultNotifyQuery(SignResultNotifyQueryParam& mParam);
	int doSignResultNotifyQuery(SignResultNotifyQueryParam& mParam, string &strSid);

///****************************************
/// @brief 支付推进
/// @param mParam				:	PayForwardParam 结构体
/// @return 
///  -  0 : 成功 
///  - !0 : 失败 
///****************************************
	int doPayForward(PayForwardParam& mParam);
	int doPayForward(PayForwardParam& mParam, string &strSid);

///****************************************
// @brief 心跳接口
// @return
//  -  0: 成功
//  - !0: 失败
///****************************************
    int  doHeartBeat();

  //------------------------------------------------//   
	//               Get Message Interface             //
	//------------------------------------------------//   
	  
///****************************************
/// @brief  获取账户信息，给到account；返回处理结果状态。 
/// @param  account		:	AccountInfo的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///****************************************
	int  getAccountInfo(AccountInfo &account);
	

///****************************************
/// @brief  获取商品信息，给到vCommodity；返回处理结果状态。 
/// @param  vCommodity 	:	CommodityInfo的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///*****************************************
	int  getCommodity(vector<CommodityInfo> &vCommodity);
	
	
///****************************************
/// @brief  获取持仓单信息，给到vHoldPosition；返回处理结果状态 
/// @param  vHoldPosition		:	 HoldPositionInfo的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///*****************************************	
	int  getPositionOrder(vector<HoldPositionInfo>& vHoldPosition);


///****************************************
/// @brief  获取限价单信息，给到vLimitOrder；返回处理结果状态
/// @param  vLimitOrder			:	LimitOrderInfo的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///****************************************
	int  getLimitOrder(vector<LimitOrderInfo>& vLimitOrder);


///****************************************
/// @brief  获取平仓信息，给到vClosePosition；返回处理结果状态 
/// @param  vClosePosition		: ClosePositionInfo的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///****************************************
	int  getClosePosition(vector<ClosePositionInfo>& vClosePosition);


///****************************************
/// @brief  获取持仓单总量，给到vTotal；返回处理结果状态
/// @param  vTotal		:	HoldPositionTotalInfo的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///****************************************
	int  getHoldPositionTotal(vector<HoldPositionTotalInfo>& vTotal);


///****************************************
/// @brief  获取用户的交割单，给到vDeliveryOrder；返回处理结果状态
/// @param  vDeliveryOrder		:	DeliveryOrderInfo的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///****************************************
	int  getDeliveryOrder(vector<DeliveryOrderInfo>& vDeliveryOrder);



///****************************************
/// @brief  获取市场开休市状态 
/// @param  nMarketStatus
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///*****************************************
	int  getMarketStatus(int& nMarketStatus);


  //------------------------------------------------// 
  //              Get Message By ID                 //
  //------------------------------------------------//  
	
///****************************************
/// @brief  根据持仓单ID，获取持仓单信息，给到pHoldPosition；返回处理结果状态
/// @param  nHoldPositionID		:	持仓单ID
/// @param  pHoldPosition			:	HoldPositionInfo类的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///****************************************
	int  getHoldPositionByID(long long nHoldPositionID, HoldPositionInfo& pHoldPosition);


///****************************************
/// @brief  根据限价单ID，获取限价单信息，给到pLimitOrder；返回处理结果状态 
/// @param  nLimitOrderID		:	限价单ID
/// @param  pLimitOrder			:	LimitOrderInfo类的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///*****************************************
	int  getLimitOrderByID(long long nLimitOrderID,  LimitOrderInfo& pLimitOrder);


///****************************************
/// @brief  根据平仓单ID，获取平仓信息，给到pClosePosition；返回处理结果状态
/// @param  nClosePositionID		:	平仓单ID
/// @param  pClosePosition			:	ClosePositionInfo类的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///*****************************************
	int  getClosePositionByID(long long nClosePositionID, ClosePositionInfo& pClosePosition);


///****************************************
/// @brief  根据商品ID，获取持仓汇总信息，给到pTotal；返回处理结果状态
/// @param  nCommodityID			:	商品ID
/// @param  nOpenDirector			:	建仓方向
/// @param  pTotal						:	HoldPositionTotalInfo类的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///*****************************************
	int  getHoldPositionTotalByCommodityID(int nCommodityID, int nOpenDirector, HoldPositionTotalInfo& pTotal);


///****************************************
/// @brief  根据商品ID，获取实时行情，给到quote；返回处理结果状态
/// @param  CommoditID		:	商品ID
/// @param  quote					:	RealTimeQuote类的实例
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///*****************************************
	int  getCommodityQuote(int CommoditID, RealTimeQuote& quote);

  //------------------------------------------------//
  //              Get Configure Message             //
  //------------------------------------------------//
  
///****************************************
/// @brief  根据商品ID，获取其市价建仓的配置信息，给到pConf；返回处理结果状态
/// @param  CommodityID		:	商品ID
/// @param  pConf					:	OpenMarketOrderConf类的实例
/// @return 
///  -  0 : 成功
///  - !0 : 失败 
///*****************************************
	int  getOpenMarketOrderConf(int CommodityID,  OpenMarketOrderConf& pConf);


///****************************************
/// @brief  根据商品ID，获取其限价建仓的配置信息，给到pConf；返回处理结果状态
/// @param  CommodityID			:	商品ID
/// @param  pConf						:	OpenLimitOrderConf类的实例
/// @return 
///  -  0 : 成功
///  - !0 : 失败 
///****************************************
	int  getOpenLimitOrderConf(int CommodityID,   OpenLimitOrderConf& pConf);


///****************************************
/// @brief  根据商品ID，获取其市价平仓的配置信息，给到pConf；返回处理结果状态
/// @param  CommodityID			:	商品ID
/// @param  pConf						:	CloseMarketOrderConf类的实例
/// @return 
///  -  0 : 成功
///  - !0 : 失败 
///****************************************
	int  getCloseMarketOrderConf(int CommodityID, CloseMarketOrderConf& pConf);


///****************************************
/// @brief  根据商品ID，获取限价平仓的配置信息，给到pConf；返回处理结果状态 
/// @param  CommodityID			:	商品ID
/// @param  pConf						:	LimitClosePositionConf类的实例
/// @return 
///  -  0 : 成功
///  - !0 : 失败 
///*****************************************
	int  getLimitClosePositionConf(int CommodityID,  LimitClosePositionConf& pConf);

///****************************************
/// @brief  根据商品ID，获取交割的配置信息，给到pConf；返回处理结果状态 
/// @param  CommodityID			:	持仓单中的商品ID
/// @param  Totalweight			:	持仓单中的总重量
/// @param  HoldPrice			:	持仓单中的持仓价
/// @param  pConf						:getOpenDeliveryOrderConf类的实例
/// @return 
///  -  0 : 成功
///  - !0 : 失败 
///*****************************************
	int  getOpenDeliveryOrderConf(int CommodityID, double Totalweight, double HoldPrice, OpenDeliveryOderConf& pConf);


///****************************************
/// @brief  根据客户端的配置类型，获取客户端的配置信息，给到ParamValue；返回处理结果 
/// @param  ParamType			:	客户端配置类型
/// @param  ParamValue		:	客户端配置标识
/// @return 
/// - MSG_STATUS_UPDATED: 成功
/// - !MSG_STATUS_UPDATED: 失败
///****************************************
	int  getClientConfig(int ParamType, double& ParamValue);

	void  loginOutNotice(int ret);

private:
	int  StartService();
	void StopService();
	void *Thread();

private:
	void processEvent(const TradeAPI::common::Event* pEvent);
	int  prcocessDispatch();

	int  InitMessage();
	int	 doReqSid(TradeAPI::common::Event *pEvent);
	int  procReqSid(int nCommand, const std::string strSID);

	int  queryAccountInfo();
	int  queryUserCommodityInfo();
	int  queryHoldPosition();
	int  queryLimitOrder();
	int  queryClosePosition();
	int  queryDeliveryOrder();
	int  queryDeliveryCommodity();
	int  queryClientConfig();
	int  queryUserCommodityFeeGroup();
	int  queryMarketStatus();
	int  queryUserDepositeGroup();

	int  queryCustomerBankInfo();

	void updateCommodityQuote(RealTimeQuote* pQuote);
	void processData();
	void processQuote();

	int  checkInitMessage();
	

	int  processHeartbeatEvent(const TradeAPI::business::HeartBeatEvent* pEvent);
	int  processLoginEvent(const TradeAPI::business::LoginEvent* pEvent);
	int  processLoginPwdUpdEvent(const TradeAPI::business::LoginPwdUpdEvent* pEvent);
	int  processFundPwdUpdEvent(const TradeAPI::business::FundPwdUpdEvent* pEvent);
	int  processTokenLoginEvent(const TradeAPI::business::TokenLoginEvent* pEvent);
	int  processCustomerAllowLoginEvent(const TradeAPI::business::CustomerAllowLoginEvent* pEvent);
	int  processUserCommodityInfoEvent(const TradeAPI::business::UserCommodityInfoEvent* pEvent);
	int  processMarketStatusEvent(const TradeAPI::business::MarketStatusQueryEvent* pEvent);
	int  processUserCommodityFeeGroupEvent(const TradeAPI::business::UserCommodityFeeGroupEvent* pEvent);
	int  processClientConfigQueryEvent(const TradeAPI::business::ClientConfigQueryEvent* pEvent);
	int  processUserDepositeGroupEvent(const TradeAPI::business::DepositeQueryEvent* pEvent);
	int  processDeliveryCommodityQueryEvent(const TradeAPI::business::DeliveryCommodityQueryEvent* pEvent);

	int  processOpenPositionEvent(const TradeAPI::business::OpenPositionEvent* pEvent);
	int  processClosePositionEvent(const TradeAPI::business::ClosePositionEvent* pEvent);
	int  processLimitOpenPositionEvent(const TradeAPI::business::LimitOpenPositionEvent* pEvent);
	int  processLimitClosePositionEvent(const TradeAPI::business::LimitClosePositionEvent* pEvent);
	int  processLimitRevokeEvent(const TradeAPI::business::LimitRevokeEvent* pEvent);
	int  processCustomerCloseManyEvent(const TradeAPI::business::CustomerCloseManyEvent* pEvent);
	int  processOpenDeliveryOrderEvent(const TradeAPI::business::OpenDeliveryEvent* pEvent);

	int  processAccountQueryEvent(const TradeAPI::business::AccountQueryEvent* pEvent);
	int  processHoldPositionOrderQueryEvent(const TradeAPI::business::HoldPositionOrderQueryEvent* pEvent);
	int  processLimitOrderQueryEvent(const TradeAPI::business::LimitOrderQueryEvent* pEvent);
	int  processClosePositionQueryEvent(const TradeAPI::business::ClosePositionQueryEvent* pEvent);
	int  processDeliveryOrderQueryEvent(const TradeAPI::business::DeliveryOrderQueryEvent* pEvent);
    int  processCustmTradeReportHoldPositionQueryEvent(const TradeAPI::business::CustmTradeReportHoldPositionQueryEvent* pEvent);
	int  processCustmTradeReportClosePositionQueryEvent(const TradeAPI::business::CustmTradeReportClosePositionQueryEvent* pEvent);
	int  processCustmTradeReportLimitOrderQueryEvent(const TradeAPI::business::CustmTradeReportLimitOrderQueryEvent* pEvent);
	int  processCustomerSelfFundFlowQueryEvent(const TradeAPI::business::CustomerSelfFundFlowQueryEvent* pEvent);


	int  processSysBulletinEvent(const TradeAPI::business::SysBulletinEvent* pEvent);

	int  processMoneyInOutEvent(const TradeAPI::business::MoneyInOutEvent* pEvent);
	int  processCustomerBankInfoEvent(const TradeAPI::business::CustomerBankInfoEvent* pEvent);
	int  processMoneyQueryEvent(const TradeAPI::business::MoneyQueryEvent* pEvent);
	int  processSignResultNotifyQueryEvent(const TradeAPI::business::SignResultNotifyQueryEvent* pEvent);
	int  processPayForwardEvent(const TradeAPI::business::PayForwardEvent* pEvent);

	int  processDisconnectEvent(const TradeAPI::business::DisconnectEvent* pEvent);

	double findFeeItem(int CommodityID, int FeeType, int FeeSubType);
	int  findCommodityIsDisplay(const int CommodityID);
	int  findCommodity(const int CommodityID, CommodityInfo &pCommodityInfo, int flag);

	int  checkCommodityRight(const long long mCommodityRight, const int mEventCode);
	int  checkOpenMarketOrder(const OpenMarketOrderParam mParam, const int mTotalWeight);
	int  checkCloseMarketOrder(const CloseMarketOrderParam mParam,const int mTotalWeight);
	int  checkCloseMarketOrderMany(const CloseMarketOrderManyParam mParam, const int mTotalWeight);
	int  checkOpenLimitOrder(const OpenLimitOrderParam mParam, const int mTotalWeight);
	int  checkCloseLimitOrder(const CloseLimitOrderParam mParam, const int mOpenDirector, const double mClosePrice);
	int  checkMoneyInOut(const MoneyInOutParam mParam);
	
	int  checkOrderSendStamp();

private:
	TradeAPI::business::TradeManager                *m_manager;
	TradeAPI::business::QuoteManager                *m_QuoteManager;
	Client                                          *m_Client;

	TradeAPI::business::BusinessMessage             *m_BusinessMessage;

private:
	UserInfo                            m_UserInfo;
	AccountInfo                         m_AccountInfo;
	CustomerBankInfo					m_CustomerBankInfo;

	map<int, CommodityInfo>             *m_mapCommodityInfo;
	map<long long, HoldPositionInfo>    *m_mapHoldPositionInfo;
	map<long long, LimitOrderInfo>      *m_mapLimitOrderInfo;
	map<long long, ClosePositionInfo>   *m_mapClosePositionInfo;
	map<long long, HoldPositionTotalInfo> *m_mapHoldPositionTotal;
	map<long long, DeliveryOrderInfo>     *m_mapDeliveryOrderInfo;
	map<int, ClientConfig>              *m_mapClientConfig;
	vector<CommodityFeeGroup>           *m_vecFeeItem;
	vector<DeliveryCommodity>     *m_vecDeliveryCommodityInfo;

	map<string, HoldPositionInfo>       *m_mapNewHoldPosition;
	map<string, LimitOrderInfo>         *m_mapNewLimitOrder;
	map<string, LimitRevokeInfo>        *m_mapNewRevokeOrder;
	map<string, LimitCloseInfo>         *m_mapNewLimitClosePosition;
	map<string, DeliveryOrderInfo>     *m_mapNewDeliveryOrder;

	map<string, int>					*m_mapReqSid;
	map<int, list<ReqHoldInfo> >		*m_mapReqHoldInfo;
	
	//int m_HeartTimeOut;
	int	m_RetryTimes;
	int m_ResTimeOut;
	time_t m_StartTimeAccUp;
	time_t m_LastTimeAccUp;
	int64_t m_OrderSendStamp;

	volatile int m_MarketStatus;

	volatile int  m_AccountStatus;
	volatile int  m_CommodityStatus;
	volatile int  m_HoldPositionStatus;
	volatile int  m_LimitOrderStatus;
	volatile int  m_ClosePositionStatus;
	volatile int  m_FeeItemStatus;
	volatile int  m_ClientConfigStatus;
	volatile int  m_CustomerBankInfoStatus;
	volatile int  m_DeliveryOrderStatus;
	volatile int  m_DeliveryCommodityStatus;

	int  m_InitFlag;
	volatile int  m_QueryHoldCnt;
	volatile int  m_QueryCloseCnt;
	volatile int  m_QueryLimitCnt;
	volatile int  m_DeliveryOrder_OpenStage;

	//about thread
	volatile bool    m_bStop;
	JMutex  m_StopMutex;
	JMutex  m_ReqMutex;
	JMutex  m_LoginMutex;
	JMutex  m_SysStatusMutex;
	JMutex  m_DataStatusMutex;

	//login flag;
	volatile int		m_LoginStatus;
	volatile int		m_LifeStage; // 1--born by init()  2--grow by login() 0--dead by release()
	volatile int		m_bIsConnect;  // true--LoginRes  false--DisCon
	volatile bool		m_bIsRelease;  //ture--Release    false--Init
	volatile bool		m_bIsLogin; // true--Login  false--UnLogin
	volatile bool		m_bFirstLogin; // true--FirstLogin  false--UnFirstLogin
	volatile bool		m_NetStatus;
	volatile bool		m_UpBalance;

	//Session
	int		m_fd;
	int		m_port;

};
#endif //__TRADE_INTERFACE_H__



