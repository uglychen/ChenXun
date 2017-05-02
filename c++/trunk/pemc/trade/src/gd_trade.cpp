#include "gd_trade.h"
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "enum.h"
#include "zlog.h"
#include <limits>       // std::numeric_limits
using namespace std;

std::string getime()
{
	char dest[32] = { 0 };
	time_t sec;
	time(&sec);
	strftime(dest, sizeof(dest), "%d%H%M%S:000000", localtime(&sec));
	string tmp = dest;
	return tmp;
}

int gd_trade::time_to_str(const long long second, char* dest, int len)
{
	if (dest == NULL || len < 20)
	{
		return -1;
	}

	memset(dest, 0, len);

	time_t sec = second;
	if (sec < TS_1970_TO_1900)
	{
		snprintf(dest, len, "1900-01-01 00:00:00");
		return 1;
	}
	sec -= TS_1970_TO_1900;
	strftime(dest, len, "%Y-%m-%d %H:%M:%S", localtime(&sec));

	return 0;
}


gd_trade::gd_trade(gd_trade_callback* callback)
	: _ptr_interface(NULL)
	, _login_status(login_status_out)
	, _last_logout_status_time(0)
	, _ptr_callback(callback)
{
}

gd_trade::~gd_trade()
{

}

int gd_trade::init()
{
	if (_ptr_interface != NULL)
	{
		dzlog_warn("_ptr_interface had been new!\n");
		return 0;
	}

	_ptr_interface = new TradeInterface;
	//_money_status = MSG_STATUS_NO_UPDATE;

	int ret = init_network();
	if (ret != 0)
	{
		delete _ptr_interface;
		_ptr_interface = NULL;
		return ret;
	}

	set_logout_status();
	return ret;
}

int gd_trade::init_network()
{
	//需要从配置文件读取数据
	int ret = _ptr_interface->Init("183.62.250.19", 15880, "./"); // 根据提供的IP更改
	if (0 != ret)
	{
		dzlog_error("init network fail !\n");
		return ret;
	}
	_ptr_interface->RegisterClient(this);
	_ptr_interface->SetAppMessage("PMEC_180", "PMEC180");
	_ptr_interface->setVendorID("874987497234");
	return ret;
}

int gd_trade::login(const std::string& user_name, const std::string& password, string& sid)
{
	if (login_status_in == _login_status)
	{
		dzlog_info("%s had been login", user_name.c_str());
		return 0;
	}

	string loginacc = user_name;
	string pwd = password;
	_login_accout = user_name;
	_login_passwd = password;
	int ret = _ptr_interface->doLogin(loginacc, pwd, sid);
	if (0 == ret)
	{
		//set_login_status();
		//start_heart_beat();
		dzlog_info("login success, user name:%s", user_name.c_str());
		return ret;
	}
	else
	{
		dzlog_error("login fail, user name:%s, ret:%d", user_name.c_str(), ret);
	}
	return ret;
}

int gd_trade::login_pwd_upd(const std::string& old_pwd, const std::string& new_pwd, int market_type, string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doLoginPwdUpd(std::string(old_pwd), std::string(new_pwd), market_type, sid);
	_mutex.unlock();
	if (0 != ret)
	{
		dzlog_error("Login password update fail! ret: %d, user name = %s", ret, _login_accout.c_str());
		return ret;
	}
	else
	{
		dzlog_info("Login password update success, user name = %s, sid = %s", _login_accout.c_str(), sid.c_str());
	}
	return ret;
}

int gd_trade::fund_pwd_upd(const std::string& old_pwd, const std::string& new_pwd, string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doFundPwdUpd(std::string(old_pwd), std::string(new_pwd), sid);
	_mutex.unlock();
	if (0 != ret)
	{
		dzlog_error("fund password update fail! ret: %d, user name = %s", ret, _login_accout.c_str());
		return ret;
	}
	else
	{
		dzlog_info("fund password update success, user name = %s, sid = %s", _login_accout.c_str(), sid.c_str());
	}

	return ret;
}

int gd_trade::logout()
{
	return release();
}

int gd_trade::release()
{
	int ret = -1;
	set_logout_status();
	_login_accout.clear();
	_login_passwd.clear();
	if (_ptr_interface)
	{
		ret = _ptr_interface->Release();
		if (ret == -1)
		{
			return ret;
		}

		delete _ptr_interface;
		_ptr_interface = NULL;
	}

	return ret;
}

void gd_trade::on_callback_task(const callback_task* ptr_request)
{
	dzlog_info("On callback task, type = %d, account = %s, success = %d", ptr_request->_event_type, ptr_request->_account.c_str(), (int)ptr_request->_success);
	switch (ptr_request->_event_type)
	{
	case EVENT_CODE_LOGIN:
	{
		if (ptr_request->_success)
		{
			set_login_status();
			//产生uuid

		}
		else
		{
			set_logout_status();
		}
			
		break;
	}
	default:
	{
		break;
	}

	}

	//添加到对应的
	boost::unordered_map<std::string, callback_response>::iterator iter = _callback_result.find(ptr_request->_info._type);
	if (_callback_result.end() != iter)
	{
		_callback_result.erase(iter);
	}

	_callback_result.insert(std::pair<std::string, callback_response>(ptr_request->_info._type, ptr_request->_info));
}


void gd_trade::procTradeEvent(const TradeEvent *pEvent)
{
	//初始化事件
	dzlog_info("recv event, type = %d", pEvent->GetType());
	

	switch (pEvent->GetType())
	{
	//登录事件
	case EVENT_CODE_LOGIN:
	{
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);


		const TLoginEvent *p = (const TLoginEvent*)(pEvent);
		ProcessResult result = p->getLoginResult();
		if (Event_Ret_Success != result.RetCode)
		{
			ptr_task->_success = false;
			if (Event_Ret_DROPUSER == result.RetCode)
			{				
				ptr_task->_success = true;
			}
			else if (Event_Ret_LOGACCERR == result.RetCode)
			{
				ptr_task->_success = true;
			}
			dzlog_info("[%s] Login Fail! Ret:%d, Msg:%s", _login_accout.c_str(), result.RetCode, result.Message);
		}
		else
		{
			dzlog_info("[%s] Login success! Ret:%d, Msg:%s", _login_accout.c_str(), result.RetCode, result.Message);
			ptr_task->_info._type = "login";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode; 
			ptr_task->_info._response["Message"] = result.Message;
			
			ptr_task->_success = true;	
		}

		_ptr_callback->on_callback_task(ptr);

		break;
	}
	//行情推送事件, 目前不处理
	case EVENT_CODE_QUOTE:
	{
	/*	dzlog_info("get quote retSignal!\n");
		const TQuoteEvent *p = (const TQuoteEvent*)(pEvent);
		RealTimeQuote quote = p->getQuote();
		dzlog_info("[T] ACC[%s] QUOTE: %d,%f,%f,%f,%f,%lld", _login_accout.c_str(),quote.CommodityID,quote.BuyPrice,quote.SellPrice,quote.HighPrice,quote.LowPrice,quote.QuoteTime);*/
		break;
	}
	/////< 初始化事件
	case EVENT_CODE_INITIALIZE:
	{
		break;
	}
	//市价建仓事件
	case EVENT_CODE_OPENMARKETORDER:
	{
		dzlog_info("[T] [%s] [%s] get OPENMARKETORDER retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);


		const TOpenMarketOrderEvent *e = (const TOpenMarketOrderEvent *)pEvent;
		ProcessResult result = e->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_info("[TE] OPENMARKETORDER ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			ptr_task->_info._type = "req.openmarket";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;
			ptr_task->_info._response["HoldPositionID"] = e->getHoldPositionID();
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//市价平仓事件
	case EVENT_CODE_CLOSEMARKETORDER:
	{
		dzlog_info("[T] [%s] [%s] get CLOSEMARKETORDER retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);


		const TCloseMarketOrderEvent *e  = (const TCloseMarketOrderEvent *)pEvent;
		ProcessResult result = e->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_info("[TE] CLOSEMARKETORDER ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			ptr_task->_info._type = "req.closemarket";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;
			ptr_task->_info._response["HoldPositionID"] = e->getHoldPositionID();
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//限价建仓事件
	case EVENT_CODE_OPENLIMITORDER:
	{
		dzlog_info("[T] [%s] [%s] get OPENLIMITORDER retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);


		const TOpenLimitOrderEvent *e = (const TOpenLimitOrderEvent *)pEvent;
		ProcessResult result = e->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_info("[TE] OPENLIMITORDER ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			ptr_task->_info._type = "req.openlimit";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;
			ptr_task->_info._response["HoldPositionID"] = e->getLimitOrderID();
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//限价平仓事件
	case EVENT_CODE_LIMITCLOSEPOSITION:
	{
		dzlog_info("[T] [%s] [%s] get CLOSELIMITORDER retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TLimitClosePositionEvent *e = NULL;
		e = (const TLimitClosePositionEvent *)pEvent;
		ProcessResult result = e->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_info("[TE] CLOSELIMITORDER ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			ptr_task->_info._type = "req.closelimit";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;
			ptr_task->_info._response["HoldPositionID"] = e->getHoldPositionID();
		}
		_ptr_callback->on_callback_task(ptr);

		break;
	}
	//撤销限价事件
	case EVENT_CODE_LIMITREVOKE:
	{
		dzlog_info("[T] [%s] [%s] get LIMITREVOKE retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TRevokeLimitOrderEvent *e = NULL;
		e = (const TRevokeLimitOrderEvent *)pEvent;
		ProcessResult result = e->getResult();
		if (Event_Ret_Success != e->getResult().RetCode)
		{
			dzlog_info("[TE] LIMITREVOKE ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			ptr_task->_info._type = "req.limitrevoke";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;
			ptr_task->_info._response["HoldPositionID"] = e->getLimitOrderID();
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//系统公告事件, 推送
	case EVENT_CODE_SYSBULLETIN:
	{
		dzlog_info("get SYSBULLETIN!\n");

		const TSysBulletinEvent *p = (const TSysBulletinEvent*)(pEvent);
		SysBulletinInfo mSysBulletinInfo = p->getSysBulletin();

		dzlog_info("[T] LoginID:%d, BulletinID:%lld, TradeMode:%d, BulletinType:%d, BulletinPriorty:%d", mSysBulletinInfo.LoginID, \
			mSysBulletinInfo.BulletinID, mSysBulletinInfo.TradeMode, mSysBulletinInfo.BulletinType, mSysBulletinInfo.BulletinPriorty);
		dzlog_info("BulletinHeader:%s, BulletinContent:%s", mSysBulletinInfo.BulletinHeader, mSysBulletinInfo.BulletinContent);
		dzlog_info("PublisherName:%s, BulletinMethod:%d", mSysBulletinInfo.PublisherName, mSysBulletinInfo.BulletinMethod);

		break;
	}
	//断连事件, 推送
	case EVENT_CODE_DISCONNECT:
	{
		dzlog_info("[T] [%s] [%s] get DISCONNECT retSignal !", getime().c_str(), _login_accout.c_str());

		const TDisConnectEvent *e = (const TDisConnectEvent*)(pEvent);
		if (Event_Ret_Success != e->getResult().RetCode)
		{
			dzlog_info("[TE] ACC[%s] DISCONNECT ERROR! result code = %d, msg = %s", _login_accout.c_str(), e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			//set_logout_status();
		}

		break;
	}
	//限价成交事件, 推送
	case EVENT_CODE_SYSBUL_LIMITCLOSE:
	{
		dzlog_info("get SYSBULLETIN_LIMITCLOSE!\n");
		const TSysBulletinEvent *p = (const TSysBulletinEvent*)(pEvent);
		SysBulletinInfo mSysBulletinInfo = p->getSysBulletin();
		
		dzlog_info("[T] LoginID:%d, BulletinID:%lld, TradeMode:%d, BulletinType:%d, BulletinPriorty:%d", mSysBulletinInfo.LoginID, \
			mSysBulletinInfo.BulletinID, mSysBulletinInfo.TradeMode, mSysBulletinInfo.BulletinType, mSysBulletinInfo.BulletinPriorty);
		dzlog_info("BulletinHeader:%s, BulletinContent:%s", mSysBulletinInfo.BulletinHeader, mSysBulletinInfo.BulletinContent);
		dzlog_info("PublisherName:%s, BulletinMethod:%d", mSysBulletinInfo.PublisherName, mSysBulletinInfo.BulletinMethod);
		break;
	}
	//斩仓事件, 推送
	case EVENT_CODE_SYSBUL_LIQUIDATION:
	{
		dzlog_info("get SYSBULLETIN_LIQUIDATION!\n");
		const TSysBulletinEvent *p = (const TSysBulletinEvent*)(pEvent);
		SysBulletinInfo mSysBulletinInfo = p->getSysBulletin();

		dzlog_info("[T] LoginID:%d, BulletinID:%lld, TradeMode:%d, BulletinType:%d, BulletinPriorty:%d", mSysBulletinInfo.LoginID, \
			mSysBulletinInfo.BulletinID, mSysBulletinInfo.TradeMode, mSysBulletinInfo.BulletinType, mSysBulletinInfo.BulletinPriorty);
		dzlog_info("BulletinHeader:%s, BulletinContent:%s", mSysBulletinInfo.BulletinHeader, mSysBulletinInfo.BulletinContent);
		dzlog_info("PublisherName:%s, BulletinMethod:%d", mSysBulletinInfo.PublisherName, mSysBulletinInfo.BulletinMethod);
		break;
	}
	//出入金事件
	case EVENT_CODE_MONEYINOUT:
	{

		dzlog_info("get MONEY_INOUT!\n");
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TMoneyInOutEvent *e = (const TMoneyInOutEvent*)(pEvent);
		MoneyInOutInfo mMoneyInOutInfo = e->getMoneyInoutResult();
		if (Event_Ret_Success != mMoneyInOutInfo.RetCode)
		{
			dzlog_info("[TE] MONEYINOUT ERROR! result code = %d, msg = %s", mMoneyInOutInfo.RetCode, mMoneyInOutInfo.Message);
		}
		else
		{
			ptr_task->_info._type = "req.moneyinout";
			ptr_task->_info._sid = mMoneyInOutInfo.StrSid;
			ptr_task->_info._response["RetCode"] = mMoneyInOutInfo.RetCode;
			ptr_task->_info._response["LocalSid"] = mMoneyInOutInfo.LocalSid;
			ptr_task->_info._response["BankSid"] = mMoneyInOutInfo.BankSid;
			ptr_task->_info._response["NotifyUrl"] = mMoneyInOutInfo.NotifyUrl;
			ptr_task->_info._response["Message"] = mMoneyInOutInfo.Message;
			ptr_task->_info._response["Ticket"] = mMoneyInOutInfo.Ticket;
		}
		dzlog_info("[T] MONEYINOUT Info localsid = %s, banksid = %s, notifyurl = %s, ticket=%s", mMoneyInOutInfo.LocalSid, mMoneyInOutInfo.BankSid, mMoneyInOutInfo.NotifyUrl, mMoneyInOutInfo.Ticket);

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//可用资金查询事件
	case EVENT_CODE_MONEYQUERY:
	{
		dzlog_info("[T] [%s] ACC[%s] \t-----------\t---------\t----------\t-----------\t get MONEY_QUERY!", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TMoneyQueryEvent *e = (const TMoneyQueryEvent*)(pEvent);
		ProcessResult  result = e->getResult();
		BourseMoneyInfo info = e-> getBourseMoneyInfo();
		if (Event_Ret_Success != result.RetCode)
		{

			dzlog_info("[TE] MONEYQUERY ERROR! result code = %d", e->getResult().RetCode);
		}
		else
		{
			ptr_task->_info._type = "req.moneyquery";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;

			ptr_task->_info._response["Amount"] = info.Amount;
			ptr_task->_info._response["AmountAvailable"] = info.AmountAvailable;
			ptr_task->_info._response["AmountFetchable"] = info.AmountFetchable;
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//心跳事件
	case EVENT_CODE_HEARTBEAT:
	{
		dzlog_info("Keep HeartBeat, user name = %s",  _login_accout.c_str());
		break;
	}
	//持仓单报表查询事件
	case EVENT_CODE_CUSTMTRADEREPORTHOLDPOSITIONQUERY:
	{
		dzlog_info("[T] [%s] [%s] get REPORTHOLDPOSITIONQUERY retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TCustmTradeReportHoldPositionQueryEvent *e = NULL;
		e = (const TCustmTradeReportHoldPositionQueryEvent *)pEvent;
		ProcessResult  result = e->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_info("[TE] REPORTHOLDPOSITIONQUERY ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			

			dzlog_info("[TE] REPORTHOLDPOSITIONQUERY total num[%d]!", e->getHoldPositionInfoNum());
			ptr_task->_info._type = "req.hodeposition";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;

			std::vector<CustmTradeReportHoldPositionInfo>::const_iterator iter = e->vec.begin();
			std::vector<CustmTradeReportHoldPositionInfo>::const_iterator end = e->vec.end();
			for (; iter != end; ++iter)
			{
				Json::Value js_tmp;
				js_tmp["tradedate"] = iter->tradedate;
				js_tmp["holdpositionid"] = iter->holdpositionid;
				js_tmp["opendate"] = iter->opendate;
				js_tmp["commodityid"] = iter->commodityid;
				js_tmp["commoditycode"] = iter->commoditycode;
				js_tmp["commodityname"] = iter->commodityname;
				js_tmp["holdquantity"] = iter->holdquantity;
				js_tmp["opendirector"] = iter->opendirector;
				js_tmp["openprice"] = iter->openprice;
				js_tmp["holdpositionpric"] = iter->holdpositionpric;
				js_tmp["slprice"] = iter->slprice;
				js_tmp["tpprice"] = iter->tpprice;
				js_tmp["settlementpl"] = iter->settlementpl;
				js_tmp["commission"] = iter->commission;
				js_tmp["latefee"] = iter->latefee;
				js_tmp["perfmargin"] = iter->perfmargin;
				js_tmp["settleprice"] = iter->settleprice;
				js_tmp["openquantity"] = iter->openquantity;
				js_tmp["holdweight"] = iter->holdweight;
				js_tmp["openweight"] = iter->openweight;
				js_tmp["commoditymode"] = iter->commoditymode;


				ptr_task->_info._response["CustmTradeReportHoldPosition"].append(js_tmp);
				dzlog_info("[TE] REPORTHOLDPOSITION:建仓单号[%lld], 建仓时间[%lld], 商品[%s], 持仓数量[%d], 持仓总量[%lf], 买/卖[%d], 建仓价[%lf], 持仓价[%lf], 手续费[%lf], 滞纳金[%lf], 履约准备金[%lf], 建仓数量[%d], 建仓总量[%lf], 结算盈亏[%lf], 结算价[%lf]", iter->holdpositionid, iter->opendate, iter->commodityname, iter->holdquantity, iter->holdweight, iter->opendirector, iter->openprice, iter->holdpositionpric, iter->commission, iter->latefee, iter->perfmargin, iter->openquantity, iter->openweight, iter->settlementpl, iter->settleprice);
			}

		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//平仓单报表查询事件
	case EVENT_CODE_CUSTMTRADEREPORTCLOSEPOSITIONQUERY:
	{
		dzlog_info("[T] [%s] [%s] get REPORTCLOSEPOSITIONQUERY retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TCustmTradeReportClosePositionQueryEvent *e = NULL;
		e = (const TCustmTradeReportClosePositionQueryEvent *)pEvent;
		ProcessResult  result = e->getResult();
		if (Event_Ret_Success != e->getResult().RetCode)
		{
			dzlog_info("[TE] REPORTCLOSEPOSITIONQUERY ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			dzlog_info("[TE] REPORTCLOSEPOSITIONQUERY total num[%d]!", e->getClosePositionInfoNum());

			ptr_task->_info._type = "req.closeorders";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;

			std::vector<CustmTradeReportClosePositionInfo>::const_iterator iter = e->vec.begin();
			std::vector<CustmTradeReportClosePositionInfo>::const_iterator end = e->vec.end();
			for (; iter != end; ++iter)
			{
				Json::Value js_tmp;
				js_tmp["tradedate"] = iter->tradedate;
				js_tmp["holdpositionid"] = iter->holdpositionid;
				js_tmp["opendate"] = iter->opendate;
				js_tmp["commodityid"] = iter->commodityid;
				js_tmp["commoditycode"] = iter->commoditycode;
				js_tmp["commodityname"] = iter->commodityname;
				js_tmp["closequantity"] = iter->closequantity;
				js_tmp["opendirector"] = iter->opendirector;
				js_tmp["openprice"] = iter->openprice;
				js_tmp["holdpositionpric"] = iter->holdpositionpric;
				js_tmp["closepositionid"] = iter->closepositionid;
				js_tmp["closedate"] = iter->closedate;
				js_tmp["closedirector"] = iter->closedirector;
				js_tmp["closeprice"] = iter->closeprice;
				js_tmp["commission"] = iter->commission;
				js_tmp["profitorloss"] = iter->profitorloss;
				js_tmp["opencommission"] = iter->opencommission;
				js_tmp["closeweight"] = iter->closeweight;
				js_tmp["commoditymode"] = iter->commoditymode;


				ptr_task->_info._response["CustmTradeReportClosePosition"].append(js_tmp);

				dzlog_info("[TE] REPORTCLOSEPOSITION:建仓单号[%lld], 建仓时间[%lld], 商品[%s], 数量[%d], 总量[%lf], 买/卖[%d], 建仓价[%lf], 持仓价[%lf], 平仓单号[%lld], 平仓时间[%lld], 买/卖[%d], 平仓价[%lf], 平仓手续费[%lf], 建仓手续费[%lf], 盈亏[%lf]", iter->holdpositionid, iter->opendate, iter->commodityname, iter->closequantity, iter->closeweight, iter->opendirector, iter->openprice, iter->holdpositionpric, iter->closepositionid, iter->closedate, iter->closedirector, iter->closeprice, iter->commission, iter->opencommission, iter->profitorloss);
			}
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	// 限价单报表查询事件
	case EVENT_CODE_CUSTMTRADEREPORTLIMITORDERQUERY:
	{
		dzlog_info("[T] [%s] [%s] get REPORTLIMITORDERQUERY retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TCustmTradeReportLimitOrderQueryEvent *e = NULL;
		e = (const TCustmTradeReportLimitOrderQueryEvent *)pEvent;
		ProcessResult  result = e->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_info("[TE] REPORTLIMITORDERQUERY ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			dzlog_info("[TE] REPORTLIMITORDERQUERY total num[%d]!", e->getLimitOrderInfoNum());

			ptr_task->_info._type = "req.limitorders";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;

			std::vector<CustmTradeReportLimitOrderInfo>::const_iterator iter = e->vec.begin();
			std::vector<CustmTradeReportLimitOrderInfo>::const_iterator end = e->vec.end();

			for (; iter != end; ++iter)
			{
				Json::Value js_tmp;
				js_tmp["tradedate"] = iter->tradedate;
				js_tmp["limitorderid"] = iter->limitorderid;
				js_tmp["createdate"] = iter->createdate;
				js_tmp["commodityid"] = iter->commodityid;
				js_tmp["commoditycode"] = iter->commoditycode;
				js_tmp["commodityname"] = iter->commodityname;
				js_tmp["openquantity"] = iter->openquantity;
				js_tmp["opendirector"] = iter->opendirector;
				js_tmp["limittype"] = iter->limittype;
				js_tmp["orderprice"] = iter->orderprice;
				js_tmp["tpprice"] = iter->tpprice;
				js_tmp["slprice"] = iter->slprice;
				js_tmp["deadline"] = iter->deadline;
				js_tmp["frozenreserve"] = iter->frozenreserve;
				js_tmp["openweight"] = iter->openweight;
				js_tmp["commoditymode"] = iter->commoditymode;


				ptr_task->_info._response["CustmTradeReportLimitOrder"].append(js_tmp);
				dzlog_info("[TE] REPORTLIMITORDER:限价单号[%lld], 下单时间[%lld], 商品[%s], 数量[%d], 总量[%lf], 买/卖[%d], 类型[%d], 限价[%lf], 止损价[%lf], 止盈价[%lf], 期限[%d], 冻结准备金[%lf]", iter->limitorderid, iter->createdate, iter->commodityname, iter->openquantity, iter->openweight, iter->opendirector, iter->limittype, iter->orderprice, iter->slprice, iter->tpprice, iter->deadline, iter->frozenreserve);
			}
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//密码修改事件
	case EVENT_CODE_PWDMODIFY:
	{
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TPwdModifyEvent *p = (const TPwdModifyEvent*)(pEvent);
		ProcessResult result = p->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_error("Modify Pwd Fail! user name = %s, sid = %s, ret = %d, msg = %s", _login_accout.c_str(), result.StrSid, result.RetCode, result.Message);
		}
		else
		{
			ptr_task->_info._type = "req.loginpwdupd"; //存在两种密码修改
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;

			dzlog_info("Modify Pwd Success, user name = %s, sid = %s, ret = %d, msg = %s", _login_accout.c_str(), result.StrSid, result.RetCode, result.Message);
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//出入金查询事件
	case EVENT_CODE_CUSTOMERSELFFUNDFLOWQUERY:
	{
		dzlog_info("[T] [%s] [%s] get CUSTOMERSELFFUNDFLOW retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TCustomerSelfFundFlowQueryEvent *e = NULL;
		e = (const TCustomerSelfFundFlowQueryEvent *)pEvent;
		ProcessResult result = e->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_info("[TE] CUSTOMERSELFFUNDFLOW ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			dzlog_info("[TE] CUSTOMERSELFFUNDFLOW total num[%d]!", e->getFundFlowRecordNum());

			ptr_task->_info._type = "req.fundflowquery";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;

			std::vector<FundFlowQueryInfo>::const_iterator iter = e->vec.begin();
			std::vector<FundFlowQueryInfo>::const_iterator end = e->vec.end();

			for (; iter != end; ++iter)
			{
				Json::Value js_tmp;
				js_tmp["FlowNumber"] = iter->FlowNumber;
				js_tmp["OperType"] = iter->OperType;
				js_tmp["BeforeAmount"] = iter->BeforeAmount;
				js_tmp["Amount"] = iter->Amount;
				js_tmp["AfterAmount"] = iter->AfterAmount;
				js_tmp["OpLoginAccount"] = iter->OpLoginAccount;
				js_tmp["OperDate"] = iter->OperDate;
				js_tmp["BankID"] = iter->BankID;
				js_tmp["OrderID"] = iter->OrderID;
				js_tmp["Remark"] = iter->Remark;
				


				ptr_task->_info._response["FundFlowQuery"].append(js_tmp);
				dzlog_info("[TE] SELFFUNDFLOW:流水号[%d], 操作类型[%d], 起始金额[%lf], 变动金额[%lf], 变后金额[%lf], 操作员[%s], 日期[%lld], 银行标识[%d], 关联单号[%d], 红包标识[%s]", iter->FlowNumber, iter->OperType, iter->BeforeAmount, iter->Amount, iter->AfterAmount, iter->OpLoginAccount, iter->OperDate, iter->BankID, iter->OrderID, iter->Remark);
			}
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//交割申请事件
	case EVENT_CODE_OPENDELIVERYORDER:
	{
		dzlog_info("[T] [%s] [%s] get OPENDELIVERYORDER retSignal !", getime().c_str(), _login_accout.c_str());
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TOpenDeliveryOrderEvent *e = (const TOpenDeliveryOrderEvent*)(pEvent);
		ProcessResult result = e->getResult();
		if (Event_Ret_Success != e->getResult().RetCode)
		{
			dzlog_info("[TE] OPENDELIVERYORDER ERROR! result code = %d, msg = %s", e->getResult().RetCode, e->getResult().Message);
		}
		else
		{
			ptr_task->_info._type = "req.opendeliverorder";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;
			ptr_task->_info._response["HoldPositionID"] = e->getHoldPositionID();
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//签约结果通知事件
	case EVENT_CODE_SIGNRESULTNOTIFYQUERY:
	{
		dzlog_info("get SIGNRESULTNOTIFYQUERY !\n");
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TSignResultNotifyQueryEvent *p = (const TSignResultNotifyQueryEvent*)(pEvent);
		ProcessResult result = p->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_info("[TE] ACC[%s] SignResultNotifyQuery Fail! Ret:%d, Msg:%s", _login_accout.c_str(), result.RetCode, result.Message);
		}
		else
		{
			ptr_task->_info._type = "req.signresultnotifyquery";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;
			dzlog_info("[TE] ACC[%s] SignResultNotifyQuery Sucess!", _login_accout.c_str());
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//支付推进事件
	case EVENT_CODE_PAYFORWARD:
	{
		dzlog_info("get PAYFORWARD !\n");
		callback_task* ptr_task = new callback_task;
		ptr_task->_account = _login_accout; //这个地方要注意， 可能会出现线程不安全情况
		ptr_task->_event_type = pEvent->GetType();
		boost::shared_ptr<proc_task> ptr = boost::shared_ptr<proc_task>(ptr_task);

		const TPayForwardEvent *p = (const TPayForwardEvent*)(pEvent);
		ProcessResult result = p->getResult();
		if (Event_Ret_Success != result.RetCode)
		{
			dzlog_info("[TE] ACC[%s] PayForward Fail! Ret:%d, Msg:%s", _login_accout.c_str(), result.RetCode, result.Message);
		}
		else
		{
			ptr_task->_info._type = "req.payforward";
			ptr_task->_info._sid = result.StrSid;
			ptr_task->_info._response["RetCode"] = result.RetCode;
			ptr_task->_info._response["Message"] = result.Message;
			dzlog_info("[TE] ACC[%s] PayForward Sucess!", _login_accout.c_str());
		}

		_ptr_callback->on_callback_task(ptr);
		break;
	}
	//修改或查询客户是否可以登录
	case EVENT_CODE_CUSTOMERALLOWLOGIN:
	{
		/* const TCustomerAllowLoginEvent *p = (const TCustomerAllowLoginEvent*)(pEvent);
		 AllowLoginInfo info = p->getAllowLoginInfo();
		 if (Event_Ret_Success != info.RetCode)
		 {
		 	dzlog_info("[TE] ACC[%s] CUSTOMERALLOWLOGIN Fail! Ret:%d, Msg:%s", _login_accout.c_str(), info.RetCode, info.Message);
		 }
		 else
		 {

		 }*/
		break;
	}
	default:
	{
		dzlog_info("[TE] get unknow RETEVENT:%d", pEvent->GetType());
		break;
	}
	}

}

bool gd_trade::get_callback_response(const std::string& type, const std::string& sid, Json::Value& response)
{
	bool ret = false;
	boost::unordered_map<std::string, callback_response>::iterator iter;

	_mutex.lock();
	iter = _callback_result.find(type);
	if (_callback_result.end() != iter && iter->second._sid == sid)
	{
		response = iter->second._response;
		ret = true;
	}
	_mutex.unlock();

	return ret;
}



int gd_trade::request_customer_allow_login(AllowLoginParam &mParam, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doCustomerAllowLogin(mParam, sid);
	_mutex.unlock();
	/*if (RET_NOT_LOGIN == ret || RET_DROPDOWN == ret)
	{
		dzlog_info("Do customer allow login success, need relogin, user name = %s, sid = %s", _login_accout.c_str(), sid.c_str());
	}
	else*/ if(0 != ret)
	{
		dzlog_error("Do customer allow login fail, user name = %s", _login_accout.c_str());
	}

	return ret;
}

int gd_trade::get_account_info(AccountInfo& account)
{
	int ret = _ptr_interface->getAccountInfo(account);
	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_error("Get account information  fail! user name = %s, ret = %d",  _login_accout.c_str(), ret);
	}
	else
	{
		dzlog_info("Get account information success,\n\tAccount=%s, LoginAccount=%s, CustomerName=%s, Amount:%f, OpenProfit:%f, FrozenReserve:%f, RiskRate:%f, NAVPrice:%f, PerformanceReserve:%f", \
			account.Account, account.LoginAccount, account.CustomerName, \
			account.Amount, account.OpenProfit, account.FrozenReserve, account.RiskRate, \
			account.NAVPrice, account.PerformanceReserve);
	}

	return ret;
}

int gd_trade::get_market_status()
{
	int marketstatus;
	_ptr_interface->getMarketStatus(marketstatus);
	dzlog_info("Get market status: %d", marketstatus);
	return marketstatus;
}


int gd_trade::get_open_market_order_conf(const int CommodityID, OpenMarketOrderConf &pConf)
{
	int ret = _ptr_interface->getOpenMarketOrderConf(CommodityID, pConf);
	if (RET_SUCCEED != ret)
	{
		dzlog_error("Get open market order conf fail! user name = %s, ret = %d", _login_accout.c_str(), ret);
	}
	else
	{
		dzlog_info("Get open market order conf success, user name = %s", _login_accout.c_str());
		dzlog_info("CommodityID: %d, MinQuantity: %6.3f, MaxQuantity: %6.3f, MinTotalWeight: %6.3f, MaxTotalWeight: %6.3f", pConf.CommodityID, pConf.MinQuantity, pConf.MaxQuantity, pConf.MinTotalWeight, pConf.MaxTotalWeight);
		dzlog_info("DefaultTradeRange: %6.3f, MinTradeRange: %6.3f, MaxTradeRange: %6.3f", pConf.DefaultTradeRange, pConf.MinTradeRange, pConf.MaxTradeRange);
		dzlog_info("AgreeUnit: %6.3f, CommodityMode: %s, WeightStep: %6.4f, WeightRadio: %6.3f", pConf.AgreeUnit, pConf.CommodityMode == NEW_COMMODITY ? "新商品" : "旧商品", pConf.WeightStep, pConf.WeightRadio);
	}
	
	return ret;
}

int gd_trade::get_close_market_order_conf(const int CommodityID, CloseMarketOrderConf &pConf)
{
	int ret = _ptr_interface->getCloseMarketOrderConf(CommodityID, pConf);
	if (0 != ret)
	{
		dzlog_error("Get close market order conf fail! user name = %s, ret = %d", _login_accout.c_str(), ret);
		return ret;
	}
	else
	{

		dzlog_info("Get close market order conf success, user name = %s", _login_accout.c_str());
		dzlog_info("CommodityID: %d, MinQuantity: %6.3f, MaxQuantity: %6.3f, MinTotalWeight: %6.3f, MaxTotalWeight: %6.3f", pConf.CommodityID, pConf.MinQuantity, pConf.MaxQuantity, pConf.MinTotalWeight, pConf.MaxTotalWeight);
		dzlog_info("DefaultTradeRange: %6.3f, MinTradeRange: %6.3f, MaxTradeRange: %6.3f", pConf.DefaultTradeRange, pConf.MinTradeRange, pConf.MaxTradeRange);
		dzlog_info("AgreeUnit: %6.3f, CommodityMode: %s, WeightStep: %6.4f, WeightRadio: %6.3f", pConf.AgreeUnit, pConf.CommodityMode == NEW_COMMODITY ? "新商品" : "旧商品", pConf.WeightStep, pConf.WeightRadio);

	}
	return ret;
}

int gd_trade::get_open_limit_order_conf(const int CommodityID, OpenLimitOrderConf &pConf)
{
	int ret = _ptr_interface->getOpenLimitOrderConf(CommodityID, pConf);
	if (0 != ret)
	{
		dzlog_info("[TE] ACC[%s] GetOpenLimitOrderConf fail! ret = %d", _login_accout.c_str(), ret);
		return ret;
	}
	else
	{

	}

	dzlog_info("*********** OpenLimitOrderConf ************\n");
	dzlog_info("CommodityID: %d, Min: %6.3f, Max: %6.3f, MinTotalWeight: %6.3f, MaxTotalWeight: %6.3f", pConf.CommodityID, pConf.MinQuantity, pConf.MaxQuantity, pConf.MinTotalWeight, pConf.MaxTotalWeight);
	dzlog_info("LimitSpread: %6.3f, FixSpread: %6.3f, TPSpread: %6.3f, SLSpread: %6.3f", pConf.LimitSpread, pConf.FixSpread, pConf.TPSpread, pConf.SLSpread);
	dzlog_info("MinPriceUnit: %6.3f, AgreeUnit: %6.3f\n, CommodityMode: %s, WeightStep: %6.4f, WeightRadio: %6.3f", pConf.MinPriceUnit, pConf.AgreeUnit, pConf.CommodityMode == NEW_COMMODITY ? "新商品" : "旧商品", pConf.WeightStep, pConf.WeightRadio);

	return ret;
}

int gd_trade::get_limit_close_position_conf(const int CommodityID, LimitClosePositionConf &pConf)
{
	int ret = _ptr_interface->getLimitClosePositionConf(CommodityID, pConf);
	if (0 != ret)
	{
		dzlog_info("[TE] ACC[%s] GetCloseLimitOrderConf fail! ret = %d", _login_accout.c_str(), ret);
		return ret;
	}

	dzlog_info("*********** LimitClosePositionConf ************\n");
	dzlog_info("CommodityID: %d, TPSpread: %6.3f, SLSpread: %6.3f", pConf.CommodityID, pConf.TPSpread, pConf.SLSpread);
	dzlog_info("FixedSpread: %6.3f, MinPriceUnit: %6.3f, AgreeUnit: %6.3f", pConf.FixedSpread, pConf.MinPriceUnit, pConf.AgreeUnit);
	dzlog_info("CommodityMode: %s, WeightRadio: %6.3f", pConf.CommodityMode == NEW_COMMODITY ? "新商品" : "旧商品", pConf.WeightRadio);
	return ret;
}

int gd_trade::get_open_delivery_order_conf(const int CommodityID, const double TotalWeight, const double HoldPrice, OpenDeliveryOderConf &pConf)
{
	int ret = _ptr_interface->getOpenDeliveryOrderConf(CommodityID, TotalWeight, HoldPrice, pConf);
	if (0 != ret)
	{
		dzlog_info("[TE] ACC[%s] GetOpenDeliveryOrderConf fail! ret = %d", _login_accout.c_str(), ret);
		return ret;
	}
	else
	{

	}
	dzlog_info("*********** OpenDeliveryOrderConf ************\n");
	dzlog_info("CommodityID: %d, CanDeliver:%s, AgreeUnit:%6.3f", pConf.CommodityID, pConf.CanDeliver == 1 ? "能" : "不能", pConf.AgreeUnit);
	dzlog_info("DeliveryCharge:%6.3f,DeliCommCnt:%d", pConf.DeliveryCharge, pConf.DeliCommCnt);
	for (int i = 0; i < pConf.DeliCommCnt; i++)
	{
		dzlog_info("DeliveryCommodityID:%d, DeliveryCommodityName:%s, ClassID:%d", pConf.DeliveryCommodity[i].CommodityID, pConf.DeliveryCommodity[i].CommodityName, pConf.DeliveryCommodity[i].TradecommodityClassID);
		dzlog_info("DeliveryRadio:%6.3f, DeliveryCommision:%6.3f", pConf.DeliveryCommodity[i].DeliveryRadio, pConf.DeliveryCommodity[i].DeliveryRadio*pConf.DeliveryCharge);
	}
	return ret;
}

int gd_trade::query_commodity_quote_by_id(const int CommodityID, RealTimeQuote &quote)
{
	int ret = _ptr_interface->getCommodityQuote(CommodityID, quote);

	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] GetCommodityQuote Fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}

	char mDate[30 + 1];
	time_to_str(quote.QuoteTime, mDate, sizeof(mDate));
	dzlog_info("OneQUOTE: %d,%f,%f,%f,%f,%s", quote.CommodityID, quote.BuyPrice, quote.SellPrice, quote.HighPrice, quote.LowPrice, mDate);

	return ret;
}


int gd_trade::query_hold_position_by_id(const long long nHoldPositionID, HoldPositionInfo& pHoldPosition)
{

	int ret = _ptr_interface->getHoldPositionByID(nHoldPositionID, pHoldPosition);
	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] GetHoldPositionByID Fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}

	dzlog_info("[T] HoldPositionID:%lld, CommodityID:%d", pHoldPosition.HoldPositionID, pHoldPosition.CommodityID);
	dzlog_info("\t CommodityName:%s,  openType:%d,  openDirect:%s", pHoldPosition.CommodityName, pHoldPosition.OpenType, pHoldPosition.OpenDirector == 1 ? "1买入" : "2卖出");
	dzlog_info("\t Quantity:%d, OpenPrice:%6.3f, HoldPositionPrice:%6.3f", pHoldPosition.Quantity, pHoldPosition.OpenPrice, pHoldPosition.HoldPositionPrice);
	dzlog_info("\t SLLimitOrderID:%lld, SLPrice:%6.3f, TPLimitOrderID:%lld, TPPrice:%6.3f", pHoldPosition.SLLimitOrderID, pHoldPosition.SLPrice, pHoldPosition.TPLimitOrderID, pHoldPosition.TPPrice);
	dzlog_info("\t OpenProfit:%6.3f, CommissionAmount:%6.3f", pHoldPosition.OpenProfit, pHoldPosition.CommissionAmount);
	dzlog_info("\t AgreeMargin:%6.3f, Freezemargin:%6.3f, OverdueFindFund:%6.3f", pHoldPosition.AgreeMargin, pHoldPosition.Freezemargin, pHoldPosition.OverdueFindFund);

	return ret;
}

int gd_trade::query_limit_order_by_id(const long long nLimitOrderID, LimitOrderInfo& pLimitOrder)
{
	int ret = _ptr_interface->getLimitOrderByID(nLimitOrderID, pLimitOrder);
	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] GetLimitOrderByID Fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}

	char createdate[30 + 1];
	char updatedate[30 + 1];
	time_to_str(pLimitOrder.CreateDate, createdate, sizeof(createdate));
	time_to_str(pLimitOrder.UpdateDate, updatedate, sizeof(updatedate));

	dzlog_info("[T] LimitOrderID:%lld, CommodityID:%d", pLimitOrder.LimitOrderID, pLimitOrder.CommodityID);
	dzlog_info("\t CommodityName:%s,  LimitType:%d, OrderType:%d, openDirect:%s", pLimitOrder.CommodityName, pLimitOrder.LimitType, pLimitOrder.OrderType, pLimitOrder.OpenDirector == 1 ? "1买入" : "2卖出");
	dzlog_info("\t Quantity:%d, OrderPrice:%6.3f, HoldPositionPrice:%s", pLimitOrder.OpenQuantity, pLimitOrder.OrderPrice, createdate);
	dzlog_info("\t SLPrice:%6.3f, TPPrice:%6.3f, UpdateDate:%s", pLimitOrder.SLPrice, pLimitOrder.TPPrice, updatedate);
	dzlog_info("\t ExpireType:%lld, FreeszMargin:%6.3f", pLimitOrder.ExpireType, pLimitOrder.FreeszMargin);

	return ret;
}

int gd_trade::query_close_position_by_id(const long long nClosePositionID, ClosePositionInfo& pClosePosition)
{
	int ret = _ptr_interface->getClosePositionByID(nClosePositionID, pClosePosition);
	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] GetClosePositionByID Fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}

	char opendate[30 + 1];
	char closedate[30 + 1];
	time_to_str(pClosePosition.OpenDate, opendate, sizeof(opendate));
	time_to_str(pClosePosition.CloseDate, closedate, sizeof(closedate));

	dzlog_info("[T] ClosePositionID:%lld, CommodityID:%d, MemberID:%d", pClosePosition.ClosePositionID, pClosePosition.CommodityID, pClosePosition.MemberID);
	dzlog_info("\t CommodityName:%s,  OpenType:%d, CloseType:%d, CloseDirector:%s", pClosePosition.CommodityName, pClosePosition.OpenType, pClosePosition.CloseType, pClosePosition.CloseDirector == 1 ? "1买入" : "2卖出");
	dzlog_info("\t Quantity:%d, OpenPrice:%6.3f, HoldPrice:%6.3f, OpenPositionID:%lld", pClosePosition.Quantity, pClosePosition.OpenPrice, pClosePosition.HoldPrice, pClosePosition.OpenPositionID);
	dzlog_info("\t ClosePrice:%6.3f, CommissionAmount:%6.3f, OpenDate:%s, CloseDate:%s", pClosePosition.ClosePrice, pClosePosition.CommissionAmount, opendate, closedate);

	return ret;
}

//int gd_trade::query_hold_position_total_by_commodity_id(int commodity_id, const int open_director, HoldPositionTotalInfo & total)
//{
//	return 0;
//}

int gd_trade::query_hold_position_total_by_commodity_id(const int nCommodityID, const int nOpenDirector, HoldPositionTotalInfo& pTotal)
{
	int ret = _ptr_interface->getHoldPositionTotalByCommodityID(nCommodityID, nOpenDirector, pTotal);
	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] GetHoldPositionTotalByCommodityID Fail! ret: %d", ret);
		return ret;
	}

	dzlog_info("[T] CommodityID:%d, CommodityName:%s", pTotal.CommodityID, pTotal.CommodityName);
	dzlog_info("\t OpenDirector:%s,Quantity:%d, ", pTotal.OpenDirector == 1 ? "1买入" : "2卖出", pTotal.Quantity);
	dzlog_info("\t OpenPriceTotal:%6.3f, AvgOpenPrice:%6.3f, HoldPriceTotal:%6.3f, AvgHoldPrice:%6.3f", pTotal.OpenPriceTotal, pTotal.AvgOpenPrice, pTotal.HoldPriceTotal, pTotal.AvgHoldPrice);
	dzlog_info("\t ClosePrice:%6.3f, OpenProfit:%6.3f", pTotal.ClosePrice, pTotal.OpenProfit);

	return ret;
}

int gd_trade::query_hold_position_total_by_director(const int nOpenDirector, vector<HoldPositionTotalInfo> &vTotal)
{
	vTotal.clear();
	vector<HoldPositionTotalInfo> all;
	int ret = _ptr_interface->getHoldPositionTotal(all);

	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] GetHoldPositionTotalByDirector Fail! ret: %d", ret);
		return ret;
	}

	if (!all.empty())
	{
		std::vector<HoldPositionTotalInfo>::iterator iter, end;
		end = all.end();
		for (iter = all.begin(); iter != end; iter++)
		{
			if (nOpenDirector == iter->OpenDirector)
			{
				vTotal.push_back(*iter);
			}
		}

		end = vTotal.end();
		for (iter = vTotal.begin(); iter != end; iter++)
		{

			dzlog_info("[T] CommodityID:%d, CommodityName:%s", iter->CommodityID, iter->CommodityName);
			dzlog_info("\t OpenDirector:%s,Quantity:%d, ", iter->OpenDirector == 1 ? "1买入" : "2卖出", iter->Quantity);
			dzlog_info("\t OpenPriceTotal:%6.3f, AvgOpenPrice:%6.3f, HoldPriceTotal:%6.3f, AvgHoldPrice:%6.3f", iter->OpenPriceTotal, iter->AvgOpenPrice, iter->HoldPriceTotal, iter->AvgHoldPrice);
			dzlog_info("\t ClosePrice:%6.3f, OpenProfit:%6.3f", iter->ClosePrice, iter->OpenProfit);
			
		}
	}
	else
	{

	}

	return ret;
}

int gd_trade::request_quote(vector<CommodityInfo> &vCommodity)
{
	vCommodity.clear();
	int ret = _ptr_interface->getCommodity(vCommodity);
	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] requestQuote Fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}

	std::vector<CommodityInfo>::iterator vit;

	for (vit = vCommodity.begin(); vit != vCommodity.end(); vit++)
	{
		char date[30 + 1];
		time_to_str(vit->QuoteTime, date, sizeof(date));
		dzlog_info("[T] OneQUOTE: %d,%s,%f,%f,%f,%f,%s,%f,%lld,%lld,%d,%s,%d,%s,%s,%d",
			vit->CommodityID, vit->CommodityName, vit->BuyPrice, vit->SellPrice, vit->HighPrice,
			vit->LowPrice, date, vit->WeightStep, vit->CommodityRight, vit->TradeRight,
			vit->CommodityClass, vit->CommodityClassName, vit->CommodityGroupID, vit->CommodityGroupName,
			vit->CommodityMode == 1 ? "新商品" : "旧商品", vit->IsDisplay);

	}

	return ret;
}


int gd_trade::request_hold_position_total(vector<HoldPositionTotalInfo>& vTotal)
{
	vTotal.clear();
	int ret = _ptr_interface->getHoldPositionTotal(vTotal);

	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] requestHoldPositionTotal Fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}

	std::vector<HoldPositionTotalInfo>::iterator it;
	for (it = vTotal.begin(); it != vTotal.end(); it++)
	{
		dzlog_info("[T] CommodityID:%d, CommodityName:%s", it->CommodityID, it->CommodityName);
		dzlog_info("\t OpenDirector:%s,Quantity:%d, TotalWeight:%6.3f ", it->OpenDirector == 1 ? "1买入" : "2卖出", it->Quantity, it->TotalWeight);
		dzlog_info("\t OpenPriceTotal:%6.3f, AvgOpenPrice:%6.3f, HoldPriceTotal:%6.3f, AvgHoldPrice:%6.3f", it->OpenPriceTotal, it->AvgOpenPrice, it->HoldPriceTotal, it->AvgHoldPrice);
		dzlog_info("\t ClosePrice:%6.3f, OpenProfit:%6.3f", it->ClosePrice, it->OpenProfit);
	}

	return ret;
}


int gd_trade::request_hold_position(vector<HoldPositionInfo>& vHoldPosition)
{
	vHoldPosition.clear();
	int ret = _ptr_interface->getPositionOrder(vHoldPosition);
	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] requestHoldPosition Fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}

	std::vector<HoldPositionInfo>::iterator it;
	for (it = vHoldPosition.begin(); it != vHoldPosition.end(); it++)
	{
		char opendate[30 + 1];
		time_to_str(it->OpenDate, opendate, sizeof(opendate));

		dzlog_info("[T] HoldPositionID:%lld, CommodityID:%d", it->HoldPositionID, it->CommodityID);
		dzlog_info("\t CommodityName:%s,  openType:%d,  openDirect:%s", it->CommodityName, it->OpenType, it->OpenDirector == 1 ? "1买入" : "2卖出");
		dzlog_info("\t Quantity:%d, Weight:%f, OpenPrice:%6.3f, HoldPositionPrice:%6.3f", it->Quantity, it->TotalWeight, it->OpenPrice, it->HoldPositionPrice);
		dzlog_info("\t ClosePrice:%6.3f, SLLimitOrderID:%lld, SLPrice:%6.3f, TPLimitOrderID:%lld, TPPrice:%6.3f", it->ClosePrice, it->SLLimitOrderID, it->SLPrice, it->TPLimitOrderID, it->TPPrice);
		dzlog_info("\t OpenProfit:%6.3f, CommissionAmount:%6.3f, OpenDate:%s", it->OpenProfit, it->CommissionAmount, opendate);
		dzlog_info("\t AgreeMargin:%6.3f, Freezemargin:%6.3f, OverdueFindFund:%6.3f", it->AgreeMargin, it->Freezemargin, it->OverdueFindFund);
	}

	return ret;
}


int gd_trade::request_limit_orders(vector<LimitOrderInfo>& vLimitOrder)
{
	vLimitOrder.clear();

	int ret = _ptr_interface->getLimitOrder(vLimitOrder);

	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] requestLimitOrders Fail! ret: %d", ret);
		return ret;
	}

	std::vector<LimitOrderInfo>::iterator it;
	for (it = vLimitOrder.begin(); it != vLimitOrder.end(); it++)
	{
		char createdate[30 + 1];
		char updatedate[30 + 1];
		time_to_str(it->CreateDate, createdate, sizeof(createdate));
		time_to_str(it->UpdateDate, updatedate, sizeof(updatedate));

		dzlog_info("[T] LimitOrderID:%lld, CommodityID:%d", it->LimitOrderID, it->CommodityID);
		dzlog_info("\t CommodityName:%s,  LimitType:%d, OrderType:%d, openDirect:%s", it->CommodityName, it->LimitType, it->OrderType, it->OpenDirector == 1 ? "1买入" : "2卖出");
		dzlog_info("\t Quantity:%d, Weight:%6.3f, OrderPrice:%6.3f, CreateDate:%s", it->OpenQuantity, it->TotalWeight, it->OrderPrice, createdate);
		dzlog_info("\t SLPrice:%6.3f, TPPrice:%6.3f, UpdateDate:%s", it->SLPrice, it->TPPrice, updatedate);
		dzlog_info("\t ExpireType:%lld, FreeszMargin:%f, DealStatus:%d", it->ExpireType, it->FreeszMargin, it->DealStatus);
	}

	return ret;
}


int gd_trade::request_close_orders(vector<ClosePositionInfo>& vClosePosition)
{
	vClosePosition.clear();
	int ret = _ptr_interface->getClosePosition(vClosePosition);
	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] requestCloseOrder Fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}

	std::vector<ClosePositionInfo>::iterator it;
	for (it = vClosePosition.begin(); it != vClosePosition.end(); it++)
	{
		char opendate[30 + 1];
		char closedate[30 + 1];
		time_to_str(it->OpenDate, opendate, sizeof(opendate));
		time_to_str(it->CloseDate, closedate, sizeof(closedate));

		dzlog_info("[T] ClosePositionID:%lld, CommodityID:%d, MemberID:%d", it->ClosePositionID, it->CommodityID, it->MemberID);
		dzlog_info("\t CommodityName:%s,  OpenType:%d, CloseType:%d, CloseDirector:%s", it->CommodityName, it->OpenType, it->CloseType, it->CloseDirector == 1 ? "1买入" : "2卖出");
		dzlog_info("\t Quantity:%d, Weight:%6.3f, OpenPrice:%6.3f, HoldPrice:%6.3f, OpenPositionID:%lld", it->Quantity, it->TotalWeight, it->OpenPrice, it->HoldPrice, it->OpenPositionID);
		dzlog_info("\t ClosePrice:%6.3f, CommissionAmount:%6.3f, OpenDate:%s, CloseDate:%s", it->ClosePrice, it->CommissionAmount, opendate, closedate);
		
	}

	return ret;
}

int gd_trade::request_delivery_order(vector<DeliveryOrderInfo>& vDeliveryOrder)
{
	vDeliveryOrder.clear();
	int ret = _ptr_interface->getDeliveryOrder(vDeliveryOrder);
	if (MSG_STATUS_UPDATED != ret)
	{
		dzlog_info("[TE] requestDeliveryOrder Fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}

	int counter = 0;
	std::vector<DeliveryOrderInfo>::iterator it;
	for (it = vDeliveryOrder.begin(); it != vDeliveryOrder.end(); it++)
	{
		counter++;
		char deliverydate[30 + 1];
		char applydate[30 + 1];
		char deliverydeadline[30 + 1];
		time_to_str(it->DeliveryDate, deliverydate, sizeof(deliverydate));
		time_to_str(it->ApplyDate, applydate, sizeof(applydate));
		time_to_str(it->DeliveryDeadline, deliverydeadline, sizeof(deliverydeadline));

		dzlog_info("[T] CNT[%d], DeliveryOrderID:%lld, CommodityID:%d, HoldPositionID:%lld", counter, it->DeliveryOrderID, it->CommodityID, it->HoldpositionID);
		dzlog_info("\t DeliveryCommodityName:%s, DeliveryPrice:%6.3f, Quantity:%d, TotalWeight:%6.3f, Amount:%6.3f, Commission:%6.3f", it->DeliveryCommodityName, it->DeliveryPrice, it->Quantity, it->TotalWeight, it->Amount, it->Commission);
		dzlog_info("\t ApplyDate:%s, DeliveryDate:%s, DeliveryDeadline:%s, DeliveryStatus:%d,DeliveryType:%s", applydate, deliverydate, deliverydeadline, it->DeliveryStatus, it->DeliveryType == 0 ? "买方交割" : "未知类型");
		
	}
	return ret;
}

int gd_trade::request_open_market_order(OpenMarketOrderParam &Order, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doOpenMarketOrder(Order, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnOpenMarketOrder fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

int gd_trade::request_close_market_order(CloseMarketOrderParam &Order, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doCloseMarketOrder(Order, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnCloseMarketOrder fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

int gd_trade::request_open_limit_order(OpenLimitOrderParam &Order, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doOpenLimitOrder(Order, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnOpenLimitOrder fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

int gd_trade::request_limit_close_position(CloseLimitOrderParam &Order, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doLimitClosePosition(Order, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnLimitClosePosition fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

int gd_trade::request_custm_trade_report_close_position(ReportQueryParam &param, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doCustmTradeReportClosePositionQuery(param, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnCustmTradeReportClosePosition fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

int gd_trade::request_custm_trade_report_hold_position(ReportQueryParam &param, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doCustmTradeReportHoldPositionQuery(param, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnCustmTradeReportHoldPosition fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

int gd_trade::request_custm_trade_report_limit_order(ReportQueryParam &param, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doCustmTradeReportLimitOrderQuery(param, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnCustmTradeReportLimitOrder fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

//
//int gd_trade::request_customer_self_fund_flow_query(FundFlowQueryParam &param, std::string& sid)
//{
//	_mutex.lock();
//	int ret = _ptr_interface->doCustomerSelfFundFlowQuery(param, sid);
//	_mutex.unlock();
//	if (Order_Ret_Success != ret)
//	{
//		dzlog_info("[TE] doCustomerSelfFundFlowQuery fail! ret: %d", ret);
//		return ret;
//	}
//	else
//	{
//
//	}
//	return ret;
//}

int gd_trade::request_customer_self_fund_flow_query(FundFlowQueryParam &param, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doCustomerSelfFundFlowQuery(param, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnCustomerSelfFundFlowQuery fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

int gd_trade::request_close_market_order_many(CloseMarketOrderManyParam &Order, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doCloseMarketOrderMany(Order, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnCloseMarketOrderMany fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

int gd_trade::request_limit_revoke(LimitRevokeParam &Order, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doLimitRevoke(Order, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnLimitRevoke fail! ret: %d", ret);
		return ret;
	}
	return ret;
}

int gd_trade::request_money_in_out(MoneyInOutParam &Order, std::string& sid)
{
	//int time = 5;
	//while (MSG_STATUS_UPDATED != _money_status) 
	//{
	//	//不能等待，有就是有，没有就是没有会减缓服务器的性能的
	//	sleep(1);
	//	if (0 == (--time)) 
	//	{
	//		dzlog_info("[TE] Please QueryMoney First!\n");
	//		return -1;
	//	}
	//}
	_mutex.lock();
	int ret = _ptr_interface->doMoneyInOut(Order, sid);
	_mutex.unlock();

	//if (Order_Ret_Success != ret)
	//{
	//	dzlog_info("[TE] OnMoneyInOut fail! ret: %d", ret);
	//}
	//else
	//{

	//}
	//_money_status = MSG_STATUS_NO_UPDATE;
	return ret;
}

int gd_trade::request_money_query(std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doMoneyQuery(sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnMoneyQuery fail! ret: %d", ret);
	}

	return ret;
}

int gd_trade::request_open_delivery_order(OpenDeliveryOrderParam &Order, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doOpenDeliveryOrder(Order, sid);
	_mutex.unlock();
	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnOpenDeliveryOrder fail! ret: %d", ret);
		return ret;
	}
	else
	{

	}
	return ret;
}

int gd_trade::request_sign_result_notify_query(SignResultNotifyQueryParam &Param, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doSignResultNotifyQuery(Param, sid);
	_mutex.unlock();

	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnSignResultNotifyQuery fail! ret: %d", ret);
	}
	else
	{

	}

	return ret;
}

int gd_trade::request_pay_forward(PayForwardParam &mParam, std::string& sid)
{
	_mutex.lock();
	int ret = _ptr_interface->doPayForward(mParam, sid);
	_mutex.unlock();

	if (Order_Ret_Success != ret)
	{
		dzlog_info("[TE] OnPayForward fail! ret: %d", ret);
	}
	else
	{

	}

	return ret;
}

int gd_trade::get_curr_status() 
{
	return _login_status;
}

void gd_trade::start_heart_beat()
{
	if (_login_status == login_status_in)
	{
		_mutex.lock();
		int ret = _ptr_interface->doHeartBeat();
		_mutex.unlock();
		if (0 != ret)
		{
			dzlog_error("Heart beat fail!, user name = %s, ret = %d", _login_accout.c_str(), ret);
			set_logout_status();
		}
		else
		{
			dzlog_info("Heart beat success, user name = %s, login status = %d", _login_accout.c_str(), _login_status);
		}
	}
	else
	{
		dzlog_error("Heart beat, login status is logout, user name = %s, login status = %d", _login_accout.c_str(), _login_status);
	}
}

bool gd_trade::check_active()
{
	if (_login_status == login_status_out && time(NULL) - _last_logout_status_time > _out_of_time_interval)
	{
		dzlog_info("Check active, user name = %s, status = %d", _login_accout.c_str(), _login_status);
		return false;
	}
	else
	{
		return true;
	}
}

void gd_trade::set_logout_status()
{
	_login_status = login_status_out;
	_last_logout_status_time = time(NULL);
	dzlog_info("Set logout status, user name = %s", _login_accout.c_str());
}

void gd_trade::set_login_status()
{
	_login_status = login_status_in;
	_last_logout_status_time = std::numeric_limits<time_t>::max();
	dzlog_info("Set login status, user name = %s", _login_accout.c_str());
}