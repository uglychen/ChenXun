#include "proc_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "enum.h"
#include "gd_trade.h"
#include "proc_thread_select.h"
#include <jsoncpp/json.h>
#include <algorithm>
#include "zlog.h"
#include <boost/shared_ptr.hpp>
#include "proc_task.h"


request_type2int& g_request_type2int()
{
	static request_type2int data;
	return data;
}

bool request_type2int::init()
{
	_string2int.insert(std::pair<std::string, int>("login", LOGIN));		//登录
	_string2int.insert(std::pair<std::string, int>("logout", LOGOUT));	//登出
	_string2int.insert(std::pair<std::string, int>("get.accountinfo", GET_ACCNTINFO));	//获取用户信息
	_string2int.insert(std::pair<std::string, int>("get.marketstatus", GET_MARKETSTATUS));	//获取市场状态

	_string2int.insert(std::pair<std::string, int>("get.openmarketconf", GET_OPENMARKETCONF));	//根据商品ID，获取其市价建仓的配置信息
	_string2int.insert(std::pair<std::string, int>("get.closemarketconf", GET_CLOSEMARKETCONF)); //根据商品ID，获取其市价平仓的配置信息
	_string2int.insert(std::pair<std::string, int>("get.openlimitconf", GET_OPENLIMITCONF));	//根据商品ID，获取其限价建仓的配置信息
	_string2int.insert(std::pair<std::string, int>("get.closelimitconf", GET_CLOSELIMITCONF));	//根据商品ID，获取其限价平仓的配置信息
	_string2int.insert(std::pair<std::string, int>("get.opendeliverconf", GET_OPENDELIVERYCONF));	//根据商品ID，获取交割的配置信息

	_string2int.insert(std::pair<std::string, int>("req.quote", REQ_QUOTE));			//请求商品行情
	_string2int.insert(std::pair<std::string, int>("req.hodeposition", REQ_HOLDPOSITION));	//请求持仓信息
	_string2int.insert(std::pair<std::string, int>("req.limitorders", REQ_LIMITORDERS));	//请求限价单信息
	_string2int.insert(std::pair<std::string, int>("req.closeorders", REQ_CLOSEORDERS));	//请求平仓单信息
	_string2int.insert(std::pair<std::string, int>("req.holdpositiontotal", REQ_HOLDPOSITIONTOTAL));  //请求所有持仓单信息

	_string2int.insert(std::pair<std::string, int>("que.quoteid", QUE_QUOTE_ID));
	_string2int.insert(std::pair<std::string, int>("que.holdpositionid", QUE_HOLDPOSITION_ID));
	_string2int.insert(std::pair<std::string, int>("que.limitordersid", QUE_LIMITORDERS_ID));
	_string2int.insert(std::pair<std::string, int>("que.closeordersid", QUE_CLOSEORDERS_ID));
	_string2int.insert(std::pair<std::string, int>("que.holdpositiontotalid", QUE_HOLDPOSITIONTOTAL_ID));
	_string2int.insert(std::pair<std::string, int>("que.holdpositiontotaldirec", QUE_HOLDPOSITIONTOTAL_DIREC));

	_string2int.insert(std::pair<std::string, int>("req.openmarket", ON_OPENMARKET));	//市价建仓
	_string2int.insert(std::pair<std::string, int>("req.closemarket", ON_CLOSEMARKET));	//市价平仓
	_string2int.insert(std::pair<std::string, int>("req.closemaretmany", ON_CLOSEMARETMANY)); //批量平仓
	_string2int.insert(std::pair<std::string, int>("req.openlimit", ON_OPENLIMIT));	//限价建仓
	_string2int.insert(std::pair<std::string, int>("req.closelimit", ON_CLOSELIMIT)); //限价平仓
	_string2int.insert(std::pair<std::string, int>("req.limitrevoke", ON_LIMITREVOKE));
	_string2int.insert(std::pair<std::string, int>("req.moneyquery", ON_MONEYQUERY));	//
	_string2int.insert(std::pair<std::string, int>("req.moneyinout", ON_MONEYINOUT)); //出入金
	_string2int.insert(std::pair<std::string, int>("req.custmtradereportholdposition", ON_CUSTMTRADEREPORTHOLDPOSITOIN));
	_string2int.insert(std::pair<std::string, int>("req.custmtradereportcloseposition", ON_CUSTMTRADEREPORTCLOSEPOSITOIN));
	_string2int.insert(std::pair<std::string, int>("req.custmtradereportlimitorder", ON_CUSTMTRADEREPORTLIMITORDER));
	_string2int.insert(std::pair<std::string, int>("req.loginpwdupd", ON_LOGINPWDUPD));	//修改密码
	_string2int.insert(std::pair<std::string, int>("req.fundpwdupd", ON_FUNDPWDUPD));		//修改密码
	_string2int.insert(std::pair<std::string, int>("req.fundflowquery", ON_FUNDFLOWQUERY));
	_string2int.insert(std::pair<std::string, int>("req.opendeliverorder", ON_OPENDELIVERYORDER));
	_string2int.insert(std::pair<std::string, int>("req.signresultnotifyquery", ON_SIGNRESULTNOTIFYQUERY));
	_string2int.insert(std::pair<std::string, int>("req.payforward", ON_PAYFORWARD));
	_string2int.insert(std::pair<std::string, int>("req.customerallowlogin", ON_CUSTOMERALLOWLOGIN));

	_string2int.insert(std::pair<std::string, int>("exit", EXIT));	//退出


	_string2int.insert(std::pair<std::string, int>("get.result", request_type_get_response));	//获取回调结果
	return true;
}

int request_type2int::get_type(const std::string& type)
{
	boost::unordered_map<std::string, int>::iterator iter = _string2int.find(type);
	if (_string2int.end() == iter)
	{
		return -1;
	}
	else
	{
		return iter->second;
	}
}

// #include "TradeClient.h"

void* proc_thread::proc_thread_func(void *param)
{
	dzlog_info("create thread id = %lu", pthread_self());
	proc_thread* ptr_self = (proc_thread*)(param);
	
	time_t now_time;
	while (ptr_self->_run)
	{
		time(&now_time);
		//判断是否心跳
		if (now_time - ptr_self->_last_heart_beat_time > ptr_self->_heart_beat_interval)
		{
			ptr_self->_last_heart_beat_time = now_time;
			ptr_self->heart_beat();
			//dzlog_info("do heart beat, thread id = %lu", pthread_self());
		}
		//判断是否存在无效的连接
		else if (now_time - ptr_self->_last_check_time > ptr_self->_check_time_interval)
		{
			ptr_self->_last_check_time = now_time;
			ptr_self->check_active();
			//dzlog_info("do check active, thread id = %lu", pthread_self());
		}

		//从缓冲器读取数据
		boost::shared_ptr<proc_task> task;
		if (true == ptr_self->_request_buffer.pop(task))
		{
			switch (task->get_proc_type())
			{
			case proc_task_type_request:
			{
				//处理数据
				request_task* ptr_task = dynamic_cast<request_task*>(task.get());
				if (NULL != ptr_task)
				{
					ptr_self->process_request(ptr_task);
				}
				else
				{

				}
				
				break;
			}
			case proc_task_type_callback:
			{
				callback_task* ptr_task = dynamic_cast<callback_task*>(task.get());
				if (NULL != ptr_task)
				{
					ptr_self->process_callback(ptr_task);
				}
				else
				{

				}

				break;
			}
			default:
			{
				break;
			}
			}
			
		}
		else
		{
			usleep(10);
		}		
	}

	dzlog_info("------------ end thread id = %ld ------------", pthread_self());
	return NULL;
}



proc_thread::proc_thread()
	: _max_capacity(500)
	, _init_capacity(100)
	, _thread_id(0)
	, _run(true)
	, _last_check_time(time(NULL))
	, _check_time_interval(60)   //60秒检查一次是否有失效连接
	, _last_heart_beat_time(time(NULL))
	, _heart_beat_interval(30) //30秒一个心跳
{

}

proc_thread::~proc_thread()
{
	destory();
}

bool proc_thread::init()
{
	//读取配置文件,从配置里读取数据
	if (1)
	{
		//_out_time = out_time;
		//_check_active_interval = check_active_interval;
		//_max_capacity = check_active_interval;
		//_init_capacity = check_active_interval;
	}

	//创建回复连接
	std::string uri = "failover:(tcp://172.18.15.7:61616,tcp://172.18.15.10:61616)?randomize=false";
	if (false == _ptr_producer.init(uri))
	{
		return false;
	}

	//初始化线程消息缓冲池
	if (false == _request_buffer.init())
	{
		return false;
	}

	//初始化线程互斥
	if (false == _user_interface_mutex.init())
	{
		return false;
	}

	//填充用户对象池
	try
	{
		gd_trade* pNew;
		for (unsigned long i = 0; i < _init_capacity; ++i)
		{
			pNew = new gd_trade(this);
			if (NULL != pNew)
			{
				_user_interface_store.push_back(pNew);
			}
		}
	}
	catch (const bad_alloc& e)
	{
		//
	}
	catch (const exception& e)
	{
		//
	}

	//创建线程
	_run = true;
	if (0 != pthread_create(&_thread_id, NULL, proc_thread_func, (void*)this))
	{
		return false;
	}

	return true;
}

void proc_thread::destory()
{
	//等待线程结束
	_run = false;
	if (0 != pthread_join(_thread_id, NULL))
	{

	}

	_ptr_producer.close();

	//关闭所有的所有的用户类实例
	boost::unordered_map<std::string, gd_trade*>::iterator iter = _user_interface_map.begin();
	boost::unordered_map<std::string, gd_trade*>::iterator end = _user_interface_map.end();
	for (; iter != end; ++iter)
	{
		delete iter->second;
	}
	_user_interface_map.clear();

	//关闭所有的所有的备用用户类实例
	unsigned size = _user_interface_store.size();
	for (unsigned i = 0; i < size; ++i)
	{
		delete _user_interface_store[i];
	}
	_user_interface_store.clear();
}

void proc_thread::on_callback_task(boost::shared_ptr<proc_task>& ptr_task)
{
	_request_buffer.push_front(ptr_task);
}

void proc_thread::process_request(const request_task* ptr_request)
{
	std::string key = ptr_request->key();
	
	try
	{
		Json::Value js_response;
		Json::Value js_result;		
		Json::FastWriter writer;

		//查找是否存在交易类
		gd_trade* ptr_trade = NULL;
		bool process_data = false;
		boost::unordered_map<std::string, gd_trade*>::iterator iter = _user_interface_map.find(key);
		if (_user_interface_map.end() != iter)
		{
			ptr_trade = iter->second;
			process_data = true;
		}
		else
		{
			if ("login" == ptr_request->_type)
			{
				ptr_trade = get_trade_from_store();
				if (NULL != ptr_trade)
				{
					process_data = true;
				}
				else
				{
					js_response["Finish"] = 1;
					js_response["Success"] = 0;
					js_response["ErrStr"] = "interface is full";
				}
			}
			else
			{
				js_response["Finish"] = 1;
				js_response["Success"] = 0;
				js_response["ErrStr"] = "Please login";
			}
		}

		if (process_data)
		{
			_user_interface_map.insert(std::pair<std::string, gd_trade*>(key, ptr_trade));
			if (false == request_type(ptr_trade, *ptr_request, js_response))
			{
				//printf();
			}
		}

		static const std::string access_type = "pemc.mid.trade.";
		js_result["Response"] = js_response;
		js_result["Type"] = std::string(access_type) + ptr_request->_type;
		js_result["Param"] = ptr_request->_param;

		std::string result = writer.write(js_result);
		dzlog_info("%s", result.c_str());

		//发送到对应的队列里去
		const cms::Destination* dest_send = ptr_request->_message->getCMSReplyTo();
		if (NULL != dest_send)
		{
			std::string correlation_id = ptr_request->_message->getCMSCorrelationID();
			_ptr_producer.send_message(dest_send, result, correlation_id);
		}
		else
		{
			//printf();
		}
	}
	catch (...)
	{
		//printf();
	}
}

void proc_thread::process_callback(const callback_task* ptr_request)
{
	std::string key = ptr_request->key();

	//查找是否存在交易类
	boost::unordered_map<std::string, gd_trade*>::iterator iter = _user_interface_map.find(key);
	if (_user_interface_map.end() != iter)
	{
		//登录失败删除
		if (EVENT_CODE_LOGIN == ptr_request->_event_type && false == ptr_request->_success) 
		{
			_user_interface_store.push_back(iter->second);
			_user_interface_map.erase(iter);
		}
		else if (EVENT_CODE_INITIALIZE == ptr_request->_event_type)
		{
			_user_interface_store.push_back(iter->second);
			_user_interface_map.erase(iter);
		}
		//else if(EVENT_CODE_DISCONNECT == ptr_request->_event_type && ptr_request->_success) //断线事件是否需要重新连接
		//{

		//}
		else
		{
			gd_trade* ptr_trade = iter->second;
			ptr_trade->on_callback_task(ptr_request);
		}
		
	}
	else
	{
		dzlog_warn("Process callback, type = %d, user name = %s", ptr_request->_event_type, ptr_request->_account.c_str());
	}
}

gd_trade* proc_thread::get_trade_from_store()
{
	if (_user_interface_store.empty())
	{
		//一个程序只能有3000个interface实例，按线程平均分配
		if (_user_interface_map.size() >= 300)
		{
			return NULL;
		}

		//申请新的数据
		try
		{
			return new gd_trade(this);
		}
		catch (const bad_alloc& e)
		{
			return NULL;
		}
		catch (const exception& e)
		{
			return NULL;
		}
	}
	else
	{
		gd_trade* prt_ret = _user_interface_store.back();
		_user_interface_store.pop_back();
		return prt_ret;
	}
}

void proc_thread::heart_beat()
{
	boost::unordered_map<std::string, gd_trade*>::iterator iter = _user_interface_map.begin();
	boost::unordered_map<std::string, gd_trade*>::iterator end = _user_interface_map.end();
	for (; iter != end; ++iter)
	{
		iter->second->start_heart_beat();
	}
}

void proc_thread::check_active()
{
	boost::unordered_map<std::string, gd_trade*>::iterator iter = _user_interface_map.begin();
	boost::unordered_map<std::string, gd_trade*>::iterator end = _user_interface_map.end();
	while (iter != end)
	{
		if (false == iter->second->check_active())
		{
			_user_interface_store.push_back(iter->second);
			iter = _user_interface_map.erase(iter);
			end = _user_interface_map.end();
		}
		else
		{
			++iter;
		}
	}
}

bool proc_thread::request_type(gd_trade* ptr_trade, const request_task& request, Json::Value& js_response)
{
	bool return_value = false;
	try
	{
		if (request._type.empty())
		{
			js_response["Success"] = 0;
			js_response["Finish"] = 1;
			js_response["ErrStr"] = "type is empty";
			dzlog_error("Request type is empty, type = %s, user name = %s", request._type.c_str(), request._account.c_str());
		}
		else
		{
			int type = g_request_type2int().get_type(request._type);
			if (LOGIN != type && ptr_trade->get_curr_status() != login_status_in)
			{
				js_response["Success"] = 0;
				js_response["Finish"] = 1;
				js_response["ErrStr"] = "logout";
				dzlog_error("status = %d, type = %s, user name = %s", ptr_trade->get_curr_status(), request._type.c_str(), request._account.c_str());
			}
			else
			{

				switch (type)
				{
				case LOGIN:	//登录
				{
					dzlog_info("\"LOGIN\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					Json::Value js_passwd = request._param["Passwd"];
					if (js_passwd.isNull() || !js_passwd.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Passwd\" error";
						dzlog_error("Need password, type = %s, user name = %s", request._type.c_str(), request._account.c_str());
						break;
					}


					int ret = ptr_trade->init();
					if (0 == ret)
					{
						std::string sid;
						ret = ptr_trade->login(request._account, js_passwd.asString(), sid);
						if (0 == ret)
						{
							js_response["Success"] = 1;
							js_response["sid"] = sid;
							dzlog_info("Login success, user name = %s", request._account.c_str());
						}
						else
						{
							js_response["Success"] = 0;
							dzlog_warn("Login fail, user name = %s, ret = %d", request._account.c_str(), ret);
						}

					}
					else
					{
						js_response["Success"] = 0;
						dzlog_warn("Init fail, user name = %s, ret = %d", request._account.c_str(), ret);
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case LOGOUT:	//登出
				{
					dzlog_info("\"LOGOUT\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					boost::unordered_map<std::string, gd_trade*>::iterator iter = _user_interface_map.find(ptr_trade->get_login_accout());
					if (iter->second == ptr_trade)
					{
						ptr_trade->logout();
						_user_interface_map.erase(iter);
						_user_interface_store.push_back(ptr_trade);
						js_response["Success"] = 1;

						//删除select, 看下是否会出错，登录和登出要认真测试
						g_proc_thread_select().del_select(ptr_trade->get_login_accout());
					}
					else
					{
						js_response["Success"] = 0;
						dzlog_error("Logout error, interface map is not match, type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case EXIT:
				{
					dzlog_info("\"EXIT\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					boost::unordered_map<std::string, gd_trade*>::iterator iter = _user_interface_map.find(ptr_trade->get_login_accout());
					if (iter->second == ptr_trade)
					{
						ptr_trade->logout();
						_user_interface_map.erase(iter);
						_user_interface_store.push_back(ptr_trade);
						js_response["Success"] = 1;

						//删除select, 看下是否会出错，登录和登出要认真测试
						g_proc_thread_select().del_select(ptr_trade->get_login_accout());
					}
					else
					{
						js_response["Success"] = 0;
						dzlog_error("EXIT, interface map is not match, type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case GET_ACCNTINFO:	//获取用户信息
				{
					dzlog_info("\"GET_ACCNTINFO\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					AccountInfo account_info;
					int ret = ptr_trade->get_account_info(account_info);
					if (MSG_STATUS_UPDATED == ret)
					{
						js_response["Success"] = 1;
						js_response["Account"] = account_info.Account;
						js_response["LoginAccount"] = account_info.LoginAccount;
						js_response["CustomerName"] = account_info.CustomerName;
						js_response["NAVPrice"] = account_info.NAVPrice;
						js_response["Amount"] = account_info.Amount;
						js_response["OpenProfit"] = account_info.OpenProfit;
						js_response["ExchangeReserve"] = account_info.ExchangeReserve;
						js_response["PerformanceReserve"] = account_info.PerformanceReserve;
						js_response["FrozenReserve"] = account_info.FrozenReserve;
						js_response["RiskRate"] = account_info.RiskRate;
					}
					else
					{
						js_response["Success"] = 0;
						dzlog_warn("Get account information fail, user name = %s, ret = %d", request._account.c_str(), ret);
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case GET_MARKETSTATUS: //获取市场状态
				{
					dzlog_info("\"GET_MARKETSTATUS\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					int ret = ptr_trade->get_market_status();
					js_response["Success"] = 1;
					js_response["Finish"] = 1;
					js_response["MarketStatus"] = ret;
					return_value = true;
					break;
				}
				case GET_OPENMARKETCONF: //根据商品ID，获取其市价建仓的配置信息
				{
					dzlog_info("\"GET_OPENMARKETCONF\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}

					int commodity_id = js_commodity_id.asInt();
					OpenMarketOrderConf open_market_order_conf;
					int ret = ptr_trade->get_open_market_order_conf(commodity_id, open_market_order_conf);
					if (RET_SUCCEED == ret)
					{
						js_response["Success"] = 1;
						js_response["CommodityID"] = open_market_order_conf.CommodityID;
						js_response["MinQuantity"] = open_market_order_conf.MinQuantity;
						js_response["MaxQuantity"] = open_market_order_conf.MaxQuantity;
						js_response["MinTradeRange"] = open_market_order_conf.MinTradeRange;
						js_response["MaxTradeRange"] = open_market_order_conf.MaxTradeRange;
						js_response["DefaultTradeRange"] = open_market_order_conf.DefaultTradeRange;
						js_response["AgreeUnit"] = open_market_order_conf.AgreeUnit;
						js_response["CommodityMode"] = open_market_order_conf.CommodityMode;
						js_response["WeightStep"] = open_market_order_conf.WeightStep;
						js_response["WeightRadio"] = open_market_order_conf.WeightRadio;
						js_response["MinTotalWeight"] = open_market_order_conf.MinTotalWeight;
						js_response["MaxTotalWeight"] = open_market_order_conf.MaxTotalWeight;
						js_response["DepositeRate"] = open_market_order_conf.DepositeRate;
						js_response["SpecificationRate"] = open_market_order_conf.SpecificationRate;
					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case GET_CLOSEMARKETCONF:	//根据商品ID，获取其市价平仓的配置信息
				{
					dzlog_info("\"GET_CLOSEMARKETCONF\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}

					int commodity_id = js_commodity_id.asInt();
					CloseMarketOrderConf close_market_order_conf;
					int ret = ptr_trade->get_close_market_order_conf(commodity_id, close_market_order_conf);
					if (RET_SUCCEED == ret)
					{
						js_response["Success"] = 1;
						js_response["CommodityID"] = close_market_order_conf.CommodityID;
						js_response["MinQuantity"] = close_market_order_conf.MinQuantity;
						js_response["MaxQuantity"] = close_market_order_conf.MaxQuantity;
						js_response["MinTradeRange"] = close_market_order_conf.MinTradeRange;
						js_response["MaxTradeRange"] = close_market_order_conf.MaxTradeRange;
						js_response["DefaultTradeRange"] = close_market_order_conf.DefaultTradeRange;
						js_response["AgreeUnit"] = close_market_order_conf.AgreeUnit;
						js_response["CommodityMode"] = close_market_order_conf.CommodityMode;
						js_response["WeightStep"] = close_market_order_conf.WeightStep;
						js_response["WeightRadio"] = close_market_order_conf.WeightRadio;
						js_response["MinTotalWeight"] = close_market_order_conf.MinTotalWeight;
						js_response["MaxTotalWeight"] = close_market_order_conf.MaxTotalWeight;
						js_response["DepositeRate"] = close_market_order_conf.DepositeRate;
						js_response["SpecificationRate"] = close_market_order_conf.SpecificationRate;
					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case GET_OPENLIMITCONF://根据商品ID，获取其限价建仓的配置信息
				{
					dzlog_info("\"GET_OPENLIMITCONF\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}

					int commodity_id = js_commodity_id.asInt();
					OpenLimitOrderConf open_limit_order_conf;
					int ret = ptr_trade->get_open_limit_order_conf(commodity_id, open_limit_order_conf);
					if (RET_SUCCEED == ret)
					{
						js_response["Success"] = 1;
						js_response["CommodityID"] = open_limit_order_conf.CommodityID;
						js_response["MinQuantity"] = open_limit_order_conf.MinQuantity;
						js_response["MaxQuantity"] = open_limit_order_conf.MaxQuantity;
						js_response["LimitSpread"] = open_limit_order_conf.LimitSpread;
						js_response["FixSpread"] = open_limit_order_conf.FixSpread;
						js_response["TPSpread"] = open_limit_order_conf.TPSpread;
						js_response["SLSpread"] = open_limit_order_conf.SLSpread;
						js_response["MinPriceUnit"] = open_limit_order_conf.MinPriceUnit;
						js_response["AgreeUnit"] = open_limit_order_conf.AgreeUnit;
						js_response["CommodityMode"] = open_limit_order_conf.CommodityMode;
						js_response["WeightStep"] = open_limit_order_conf.WeightStep;
						js_response["WeightRadio"] = open_limit_order_conf.WeightRadio;
						js_response["MinTotalWeight"] = open_limit_order_conf.MinTotalWeight;
						js_response["MaxTotalWeight"] = open_limit_order_conf.MaxTotalWeight;
						js_response["DepositeRate"] = open_limit_order_conf.DepositeRate;
						js_response["SpecificationRate"] = open_limit_order_conf.SpecificationRate;

					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case GET_CLOSELIMITCONF:	//根据商品ID，获取其限价平仓的配置信息
				{
					dzlog_info("\"GET_CLOSELIMITCONF\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}

					int commodity_id = js_commodity_id.asInt();
					LimitClosePositionConf limit_close_position_conf;
					int ret = ptr_trade->get_limit_close_position_conf(commodity_id, limit_close_position_conf);
					if (RET_SUCCEED == ret)
					{
						js_response["Success"] = 1;
						js_response["CommodityID"] = limit_close_position_conf.CommodityID;
						js_response["FixedSpread"] = limit_close_position_conf.FixedSpread;
						js_response["TPSpread"] = limit_close_position_conf.TPSpread;
						js_response["SLSpread"] = limit_close_position_conf.SLSpread;
						js_response["MinPriceUnit"] = limit_close_position_conf.MinPriceUnit;
						js_response["AgreeUnit"] = limit_close_position_conf.AgreeUnit;
						js_response["CommodityMode"] = limit_close_position_conf.CommodityMode;
						js_response["WeightRadio"] = limit_close_position_conf.WeightRadio;
					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case GET_OPENDELIVERYCONF: //根据商品ID，获取交割的配置信息
				{
					dzlog_info("\"GET_OPENDELIVERYCONF\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}
					int commodity_id = js_commodity_id.asInt();

					Json::Value js_hold_weight = request._param["HoldWeight"];
					if (js_hold_weight.isNull() || !js_commodity_id.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"HoldWeight\" error";
						break;
					}
					double hold_weight = js_commodity_id.asDouble();

					Json::Value js_hold_price = request._param["HoldPrice"];
					if (js_hold_price.isNull() || !js_hold_price.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"HoldPrice\" error";
						break;
					}
					double hold_price = js_hold_price.asDouble();

					OpenDeliveryOderConf open_delivery_oder_conf;
					int ret = ptr_trade->get_open_delivery_order_conf(commodity_id, hold_weight, hold_price, open_delivery_oder_conf);
					if (RET_SUCCEED == ret)
					{
						js_response["Success"] = 1;
						js_response["CommodityID"] = open_delivery_oder_conf.CommodityID;
						js_response["CanDeliver"] = open_delivery_oder_conf.CanDeliver;
						js_response["AgreeUnit"] = open_delivery_oder_conf.AgreeUnit;
						js_response["DeliveryCharge"] = open_delivery_oder_conf.DeliveryCharge;
						js_response["DeliCommCnt"] = open_delivery_oder_conf.DeliCommCnt;

						if (open_delivery_oder_conf.DeliCommCnt > 0)
						{
							Json::Value js_delivery_commodity;
							for (int i = 0; i < open_delivery_oder_conf.DeliCommCnt; ++i)
							{
								Json::Value js_tmp;
								js_tmp["CommodityID"] = open_delivery_oder_conf.DeliveryCommodity[i].CommodityID;
								js_tmp["CommodityName"] = open_delivery_oder_conf.DeliveryCommodity[i].CommodityName;
								js_tmp["TradecommodityClassID"] = open_delivery_oder_conf.DeliveryCommodity[i].TradecommodityClassID;
								js_tmp["DeliveryRadio"] = open_delivery_oder_conf.DeliveryCommodity[i].DeliveryRadio;

								js_delivery_commodity.append(js_tmp);
							}

							js_response["DeliveryCommodity"] = js_delivery_commodity;
						}

					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case REQ_QUOTE: //请求商品行情
				{
					dzlog_info("\"REQ_QUOTE\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					std::vector<CommodityInfo> commodity;
					int ret = ptr_trade->request_quote(commodity);
					if (MSG_STATUS_UPDATED == ret)
					{
						js_response["Success"] = 1;

						if (!commodity.empty())
						{
							Json::Value js_commodity;
							unsigned long size = commodity.size();
							for (unsigned long i = 0; i < size; ++i)
							{
								Json::Value js_tmp;
								js_tmp["CommodityID"] = commodity[i].CommodityID;
								js_tmp["CommodityName"] = commodity[i].CommodityName;
								js_tmp["CommodityRight"] = commodity[i].CommodityRight;
								js_tmp["TradeRight"] = commodity[i].TradeRight;
								js_tmp["AgreeUnit"] = commodity[i].AgreeUnit;
								js_tmp["Currency"] = commodity[i].Currency;
								js_tmp["MinQuoteChangeUnit"] = commodity[i].MinQuoteChangeUnit;
								js_tmp["MinPriceUnit"] = commodity[i].MinPriceUnit;
								js_tmp["FixedSpread"] = commodity[i].FixedSpread;
								js_tmp["BuyPrice"] = commodity[i].BuyPrice;
								js_tmp["SellPrice"] = commodity[i].SellPrice;
								js_tmp["HighPrice"] = commodity[i].HighPrice;
								js_tmp["LowPrice"] = commodity[i].LowPrice;
								js_tmp["QuoteTime"] = commodity[i].QuoteTime;
								js_tmp["CommodityClass"] = commodity[i].CommodityClass;
								js_tmp["CommodityClassName"] = commodity[i].CommodityClassName;
								js_tmp["CommodityMode"] = commodity[i].CommodityMode;
								js_tmp["IsDisplay"] = commodity[i].IsDisplay;
								js_tmp["CommodityGroupID"] = commodity[i].CommodityGroupID;
								js_tmp["CommodityGroupName"] = commodity[i].CommodityGroupName;
								js_tmp["WeightStep"] = commodity[i].WeightStep;
								js_tmp["WeightRadio"] = commodity[i].WeightRadio;
								js_tmp["TradeType"] = commodity[i].TradeType;
								js_tmp["SpecificationRate"] = commodity[i].SpecificationRate;
								js_tmp["SpecificationUnit"] = commodity[i].SpecificationUnit;

								js_commodity.append(js_tmp);
							}

							js_response["Commodity"] = js_commodity;
						}

					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case REQ_HOLDPOSITION: //请求持仓信息
				{
					dzlog_info("\"REQ_HOLDPOSITION\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					std::vector<HoldPositionInfo> hold_position;
					int ret = ptr_trade->request_hold_position(hold_position);
					if (MSG_STATUS_UPDATED == ret)
					{
						js_response["Success"] = 1;

						if (!hold_position.empty())
						{
							Json::Value js_hold_position;
							unsigned long size = hold_position.size();
							for (unsigned long i = 0; i < size; ++i)
							{
								Json::Value js_tmp;
								js_tmp["HoldPositionID"] = hold_position[i].HoldPositionID;
								js_tmp["CommodityID"] = hold_position[i].CommodityID;
								js_tmp["CommodityName"] = hold_position[i].CommodityName;
								js_tmp["OpenType"] = hold_position[i].OpenType;
								js_tmp["OpenDirector"] = hold_position[i].OpenDirector;
								js_tmp["Quantity"] = hold_position[i].Quantity;

								js_tmp["TotalWeight"] = hold_position[i].TotalWeight;
								js_tmp["OpenPrice"] = hold_position[i].OpenPrice;
								js_tmp["HoldPositionPrice"] = hold_position[i].HoldPositionPrice;
								js_tmp["ClosePrice"] = hold_position[i].ClosePrice;
								js_tmp["SLLimitOrderID"] = hold_position[i].SLLimitOrderID;
								js_tmp["SLPrice"] = hold_position[i].SLPrice;
								js_tmp["TPLimitOrderID"] = hold_position[i].TPLimitOrderID;
								js_tmp["TPPrice"] = hold_position[i].TPPrice;
								js_tmp["OpenProfit"] = hold_position[i].OpenProfit;
								js_tmp["CommissionAmount"] = hold_position[i].CommissionAmount;
								js_tmp["OpenDate"] = hold_position[i].OpenDate;
								js_tmp["AgreeMargin"] = hold_position[i].AgreeMargin;
								js_tmp["Freezemargin"] = hold_position[i].Freezemargin;
								js_tmp["OverdueFindFund"] = hold_position[i].OverdueFindFund;

								js_hold_position.append(js_tmp);
							}

							js_response["HoldPosition"] = js_hold_position;
						}

					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case REQ_LIMITORDERS: //请求限价单信息
				{
					dzlog_info("\"REQ_LIMITORDERS\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					std::vector<LimitOrderInfo> limit_order;
					int ret = ptr_trade->request_limit_orders(limit_order);
					if (MSG_STATUS_UPDATED == ret)
					{
						js_response["Success"] = 1;

						if (!limit_order.empty())
						{
							Json::Value js_limit_order;
							unsigned long size = limit_order.size();
							for (unsigned long i = 0; i < size; ++i)
							{
								Json::Value js_tmp;
								js_tmp["LimitOrderID"] = limit_order[i].LimitOrderID;
								js_tmp["CommodityID"] = limit_order[i].CommodityID;
								js_tmp["CommodityName"] = limit_order[i].CommodityName;
								js_tmp["LimitType"] = limit_order[i].LimitType;
								js_tmp["OrderType"] = limit_order[i].OrderType;
								js_tmp["OpenDirector"] = limit_order[i].OpenDirector;
								js_tmp["OrderPrice"] = limit_order[i].OrderPrice;
								js_tmp["SLPrice"] = limit_order[i].SLPrice;
								js_tmp["TPPrice"] = limit_order[i].TPPrice;
								js_tmp["OpenQuantity"] = limit_order[i].OpenQuantity;
								js_tmp["TotalWeight"] = limit_order[i].TotalWeight;
								js_tmp["CreateDate"] = limit_order[i].CreateDate;
								js_tmp["ExpireType"] = limit_order[i].ExpireType;
								js_tmp["UpdateDate"] = limit_order[i].UpdateDate;
								js_tmp["FreeszMargin"] = limit_order[i].FreeszMargin;
								js_tmp["DealStatus"] = limit_order[i].DealStatus;


								js_limit_order.append(js_tmp);
							}

							js_response["LimitOrder"] = js_limit_order;
						}

					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case REQ_CLOSEORDERS:	//请求平仓单信息
				{
					dzlog_info("\"REQ_CLOSEORDERS\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					std::vector<ClosePositionInfo> close_position;
					int ret = ptr_trade->request_close_orders(close_position);
					if (MSG_STATUS_UPDATED == ret)
					{
						js_response["Success"] = 1;

						if (!close_position.empty())
						{
							Json::Value js_close_position;
							unsigned long size = close_position.size();
							for (unsigned long i = 0; i < size; ++i)
							{
								Json::Value js_tmp;
								js_tmp["ClosePositionID"] = close_position[i].ClosePositionID;
								js_tmp["CommodityID"] = close_position[i].CommodityID;
								js_tmp["CommodityName"] = close_position[i].CommodityName;
								js_tmp["CloseDirector"] = close_position[i].CloseDirector;
								js_tmp["OpenPrice"] = close_position[i].OpenPrice;
								js_tmp["HoldPrice"] = close_position[i].HoldPrice;
								js_tmp["ClosePrice"] = close_position[i].ClosePrice;
								js_tmp["Quantity"] = close_position[i].Quantity;
								js_tmp["TotalWeight"] = close_position[i].TotalWeight;
								js_tmp["OpenPositionID"] = close_position[i].OpenPositionID;
								js_tmp["CommissionAmount"] = close_position[i].CommissionAmount;
								js_tmp["OpenDate"] = close_position[i].OpenDate;
								js_tmp["CloseDate"] = close_position[i].CloseDate;
								js_tmp["MemberID"] = close_position[i].MemberID;
								js_tmp["OpenType"] = close_position[i].OpenType;
								js_tmp["CloseType"] = close_position[i].CloseType;


								js_close_position.append(js_tmp);
							}

							js_response["ClosePosition"] = js_close_position;
						}

					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case REQ_HOLDPOSITIONTOTAL: //请求所有持仓单信息
				{
					dzlog_info("\"REQ_HOLDPOSITIONTOTAL\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					std::vector<HoldPositionTotalInfo> hold_position_total;
					int ret = ptr_trade->request_hold_position_total(hold_position_total);
					if (MSG_STATUS_UPDATED == ret)
					{
						js_response["Success"] = 1;

						if (!hold_position_total.empty())
						{
							Json::Value js_hold_position_total;
							unsigned long size = hold_position_total.size();
							for (unsigned long i = 0; i < size; ++i)
							{
								Json::Value js_tmp;
								js_tmp["CommodityID"] = hold_position_total[i].CommodityID;
								js_tmp["CommodityName"] = hold_position_total[i].CommodityName;
								js_tmp["OpenDirector"] = hold_position_total[i].OpenDirector;
								js_tmp["Quantity"] = hold_position_total[i].Quantity;
								js_tmp["TotalWeight"] = hold_position_total[i].TotalWeight;
								js_tmp["OpenPriceTotal"] = hold_position_total[i].OpenPriceTotal;
								js_tmp["AvgOpenPrice"] = hold_position_total[i].AvgOpenPrice;
								js_tmp["HoldPriceTotal"] = hold_position_total[i].HoldPriceTotal;
								js_tmp["AvgHoldPrice"] = hold_position_total[i].AvgHoldPrice;
								js_tmp["ClosePrice"] = hold_position_total[i].ClosePrice;
								js_tmp["OpenProfit"] = hold_position_total[i].OpenProfit;

								js_hold_position_total.append(js_tmp);
							}

							js_response["HoldPositionTotal"] = js_hold_position_total;
						}

					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case QUE_QUOTE_ID:
				{
					return_value = false;
					break;
				}
				case QUE_HOLDPOSITION_ID:
				{
					return_value = false;
					break;
				}
				case QUE_LIMITORDERS_ID:
				{
					return_value = false;
					break;
				}
				case QUE_CLOSEORDERS_ID:
				{
					return_value = false;
					break;
				}
				case QUE_HOLDPOSITIONTOTAL_ID:
				{
					return_value = false;
					break;
				}
				case QUE_HOLDPOSITIONTOTAL_DIREC:
				{
					return_value = false;
					break;
				}
				case ON_OPENMARKET:	//市价建仓
				{
					//将所有的建仓单排在第一位，进入队列时候排在第一位，开启优先队列
					//考虑时间因素
					dzlog_info("\"ON_OPENMARKET\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					OpenMarketOrderParam order;
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}
					order.nCommodityID = js_commodity_id.asInt();

					Json::Value js_open_director = request._param["OpenDirector"];
					if (js_open_director.isNull() || !js_open_director.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OpenDirector\" error";
						break;
					}
					order.nOpenDirector = js_open_director.asInt();
					if (order.nOpenDirector != OPENDIRECTOR_BUY && order.nOpenDirector != OPENDIRECTOR_SELL)
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OpenDirector\" error";
						break;
					}

					Json::Value js_quantity = request._param["Quantity"];
					if (js_quantity.isNull() || !js_quantity.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Quantity\" error";
						break;
					}
					order.nQuantity = js_quantity.asInt();


					Json::Value js_trade_range = request._param["TradeRange"];
					if (js_trade_range.isNull() || !js_trade_range.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"TradeRange\" error";
						break;
					}
					order.dbTradeRange = js_trade_range.asDouble();

					Json::Value js_weight = request._param["Weight"];
					if (js_weight.isNull() || !js_weight.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Weight\" error";
						break;
					}
					order.dbWeight = js_weight.asDouble();

					RealTimeQuote quote;
					int ret = ptr_trade->query_commodity_quote_by_id(order.nCommodityID, quote);
					if (MSG_STATUS_UPDATED == ret)
					{
						std::string sid;
						order.dbPrice = (order.nOpenDirector == OPENDIRECTOR_BUY ? quote.BuyPrice : quote.SellPrice);
						ret = ptr_trade->request_open_market_order(order, sid);
						if (Order_Ret_Success == ret)
						{
							js_response["Success"] = 1;
							js_response["Sid"] = sid;
						}
						else
						{
							js_response["Success"] = 0;
						}
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;

				}
				case ON_CLOSEMARKET: //市价平仓
				{
					dzlog_info("\"ON_CLOSEMARKET\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					CloseMarketOrderParam order;
					Json::Value js_hold_position_id = request._param["HoldPositionID"];
					if (js_hold_position_id.isNull() || !js_hold_position_id.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"HoldPositionID\" error";
						break;
					}
					order.nHoldPositionID = js_hold_position_id.asInt64();

					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}
					order.nCommodityID = js_commodity_id.asInt();

					Json::Value js_weight = request._param["Weight"];
					if (js_weight.isNull() || !js_weight.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Weight\" error";
						break;
					}
					order.dbWeight = js_weight.asDouble();

					Json::Value js_quantity = request._param["Quantity"];
					if (js_quantity.isNull() || !js_quantity.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Quantity\" error";
						break;
					}
					order.nQuantity = js_quantity.asInt();

					Json::Value js_trade_range = request._param["TradeRange"];
					if (js_trade_range.isNull() || !js_trade_range.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"TradeRange\" error";
						break;
					}
					order.nTradeRange = js_trade_range.asInt();


					RealTimeQuote quote;
					int ret = ptr_trade->query_commodity_quote_by_id(order.nCommodityID, quote);
					if (MSG_STATUS_UPDATED == ret)
					{
						std::string sid;
						//price 价格如何确定？
						//order.dbPrice = (director == OPENDIRECTOR_BUY ? quote.BuyPrice : quote.SellPrice);
						ret = ptr_trade->request_close_market_order(order, sid);
						if (Order_Ret_Success == ret)
						{
							js_response["Success"] = 1;
							js_response["Sid"] = sid;
						}
						else
						{
							js_response["Success"] = 0;
						}
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_CLOSEMARETMANY:	//批量平仓
				{
					dzlog_info("\"ON_CLOSEMARETMANY\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					CloseMarketOrderManyParam order;
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}
					order.nCommodityID = js_commodity_id.asInt();

					Json::Value js_weight = request._param["Weight"];
					if (js_weight.isNull() || !js_weight.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Weight\" error";
						break;
					}
					order.dbWeight = js_weight.asDouble();

					Json::Value js_quantity = request._param["Quantity"];
					if (js_quantity.isNull() || !js_quantity.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Quantity\" error";
						break;
					}
					order.nQuantity = js_quantity.asInt();

					Json::Value js_trade_range = request._param["TradeRange"];
					if (js_trade_range.isNull() || !js_trade_range.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"TradeRange\" error";
						break;
					}
					order.nTradeRange = js_trade_range.asInt();

					Json::Value js_close_director = request._param["CloseDirector"];
					if (js_close_director.isNull() || !js_close_director.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CloseDirector\" error";
						break;
					}
					order.nCloseDirector = js_close_director.asInt();
					if (order.nCloseDirector != OPENDIRECTOR_BUY && order.nCloseDirector != OPENDIRECTOR_SELL)
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CloseDirector\" error";
						break;
					}

					RealTimeQuote quote;
					int ret = ptr_trade->query_commodity_quote_by_id(order.nCommodityID, quote);
					if (MSG_STATUS_UPDATED == ret)
					{
						std::string sid;
						order.dbPrice = (order.nCloseDirector == OPENDIRECTOR_BUY ? quote.BuyPrice : quote.SellPrice);
						ret = ptr_trade->request_close_market_order_many(order, sid);
						if (Order_Ret_Success == ret)
						{
							js_response["Success"] = 1;
							js_response["Sid"] = sid;
						}
						else
						{
							js_response["Success"] = 0;
						}
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;

				}
				case ON_OPENLIMIT: //限价建仓
				{
					dzlog_info("\"ON_OPENLIMIT\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					OpenLimitOrderParam order;
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}
					order.nCommodityID = js_commodity_id.asInt();

					Json::Value js_expire_type = request._param["ExpireType"];
					if (js_expire_type.isNull() || !js_expire_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"ExpireType\" error";
						break;
					}
					order.nExpireType = js_expire_type.asInt();

					Json::Value js_open_director = request._param["OpenDirector"];
					if (js_open_director.isNull() || !js_open_director.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OpenDirector\" error";
						break;
					}
					order.nOpenDirector = js_open_director.asInt();
					if (order.nOpenDirector != OPENDIRECTOR_BUY && order.nOpenDirector != OPENDIRECTOR_SELL)
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OpenDirector\" error";
						break;
					}

					Json::Value js_weight = request._param["Weight"];
					if (js_weight.isNull() || !js_weight.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Weight\" error";
						break;
					}
					order.dbWeight = js_weight.asDouble();

					Json::Value js_quantity = request._param["Quantity"];
					if (js_quantity.isNull() || !js_quantity.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Quantity\" error";
						break;
					}
					order.nQuantity = js_quantity.asInt();

					Json::Value js_order_price = request._param["OrderPrice"];
					if (js_order_price.isNull() || !js_order_price.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OrderPrice\" error";
						break;
					}
					order.dbOrderPrice = js_order_price.asDouble();

					Json::Value js_sl_price = request._param["SLPrice"];
					if (js_sl_price.isNull() || !js_sl_price.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"SLPrice\" error";
						break;
					}
					order.dbSLPrice = js_sl_price.asDouble();

					Json::Value js_tp_price = request._param["TPPrice"];
					if (js_tp_price.isNull() || !js_tp_price.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"TPPrice\" error";
						break;
					}
					order.dbTPPrice = js_tp_price.asDouble();


					std::string sid;
					int ret = ptr_trade->request_open_limit_order(order, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_CLOSELIMIT:	//限价平仓
				{
					dzlog_info("\"ON_CLOSELIMIT\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					CloseLimitOrderParam order;
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}
					order.nCommodityID = js_commodity_id.asInt();

					Json::Value js_close_price = request._param["ClosePrice"];
					if (js_close_price.isNull() || !js_close_price.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"ClosePrice\" error";
						break;
					}
					order.dbClosePrice = js_close_price.asDouble();

					Json::Value js_expire_type = request._param["ExpireType"];
					if (js_expire_type.isNull() || !js_expire_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"ExpireType\" error";
						break;
					}
					order.nExpireType = js_expire_type.asInt();



					Json::Value js_hold_position_id = request._param["HoldPositionID"];
					if (js_hold_position_id.isNull() || !js_hold_position_id.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"HoldPositionID\" error";
						break;
					}
					order.nHoldPositionID = js_hold_position_id.asInt64();

					Json::Value js_sl_price = request._param["SLPrice"];
					if (js_sl_price.isNull() || !js_sl_price.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"SLPrice\" error";
						break;
					}
					order.dbSLPrice = js_sl_price.asDouble();


					Json::Value js_tp_price = request._param["TPPrice"];
					if (js_tp_price.isNull() || !js_tp_price.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"TPPrice\" error";
						break;
					}
					order.dbTPPrice = js_tp_price.asDouble();


					std::string sid;
					int ret = ptr_trade->request_limit_close_position(order, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_OPENDELIVERYORDER:
				{
					dzlog_info("\"ON_OPENDELIVERYORDER\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					OpenDeliveryOrderParam order;
					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}
					order.nCommodityID = js_commodity_id.asInt();

					Json::Value js_hold_position_id = request._param["HoldPositionID"];
					if (js_hold_position_id.isNull() || !js_hold_position_id.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"HoldPositionID\" error";
						break;
					}
					order.nHoldPositionID = js_hold_position_id.asInt64();

					Json::Value js_delivery_commodity_id = request._param["DeliveryCommodityID"];
					if (js_delivery_commodity_id.isNull() || !js_delivery_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"DeliveryCommodityID\" error";
						break;
					}
					order.nDeliveryCommodityID = js_delivery_commodity_id.asInt();


					Json::Value js_app_desc = request._param["AppDesc"];
					if (js_app_desc.isNull() || !js_app_desc.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"AppDesc\" error";
						break;
					}
					int app_desc_len = js_app_desc.asString().length();
					if (app_desc_len > MAX_APPDES_LEN - 1)
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"AppDesc\" length error";
						break;
					}
					strcpy(order.sAppDesc, js_app_desc.asString().c_str());


					std::string sid;
					int ret = ptr_trade->request_open_delivery_order(order, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_CUSTMTRADEREPORTHOLDPOSITOIN:
				{
					ReportQueryParam param;
					Json::Value js_query_date_type = request._param["QueryDateType"];
					if (js_query_date_type.isNull() || !js_query_date_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"QueryDateType\" error";
						break;
					}
					param.nQueryDateType = js_query_date_type.asInt();

					Json::Value js_begin_date = request._param["BeginDate"];
					if (js_begin_date.isNull() || !js_begin_date.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BeginDate\" error";
						break;
					}
					param.nBeginDate = js_begin_date.asInt64();

					Json::Value js_end_date = request._param["EndDate"];
					if (js_end_date.isNull() || !js_end_date.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"EndDate\" error";
						break;
					}
					param.nEndDate = js_end_date.asInt64();

					Json::Value js_begin_row = request._param["BeginRow"];
					if (js_begin_row.isNull() || !js_begin_row.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BeginRow\" error";
						break;
					}
					param.nBeginRow = js_begin_row.asInt();

					Json::Value js_end_row = request._param["EndRow"];
					if (js_end_row.isNull() || !js_end_row.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"EndRow\" error";
						break;
					}
					param.nEndRow = js_end_row.asInt();

					std::string sid;
					int ret = ptr_trade->request_custm_trade_report_hold_position(param, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_CUSTMTRADEREPORTCLOSEPOSITOIN:
				{
					dzlog_info("\"ON_CUSTMTRADEREPORTCLOSEPOSITOIN\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					ReportQueryParam param;
					Json::Value js_query_date_type = request._param["QueryDateType"];
					if (js_query_date_type.isNull() || !js_query_date_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"QueryDateType\" error";
						break;
					}
					param.nQueryDateType = js_query_date_type.asInt();

					Json::Value js_begin_date = request._param["BeginDate"];
					if (js_begin_date.isNull() || !js_begin_date.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BeginDate\" error";
						break;
					}
					param.nBeginDate = js_begin_date.asInt64();

					Json::Value js_end_date = request._param["EndDate"];
					if (js_end_date.isNull() || !js_end_date.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"EndDate\" error";
						break;
					}
					param.nEndDate = js_end_date.asInt64();

					Json::Value js_begin_row = request._param["BeginRow"];
					if (js_begin_row.isNull() || !js_begin_row.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BeginRow\" error";
						break;
					}
					param.nBeginRow = js_begin_row.asInt();

					Json::Value js_end_row = request._param["EndRow"];
					if (js_end_row.isNull() || !js_end_row.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"EndRow\" error";
						break;
					}
					param.nEndRow = js_end_row.asInt();

					std::string sid;
					int ret = ptr_trade->request_custm_trade_report_close_position(param, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;

					return_value = true;
					break;
				}
				case ON_CUSTMTRADEREPORTLIMITORDER:
				{
					dzlog_info("\"ON_CUSTMTRADEREPORTLIMITORDER\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					ReportQueryParam param;
					Json::Value js_query_date_type = request._param["QueryDateType"];
					if (js_query_date_type.isNull() || !js_query_date_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"QueryDateType\" error";
						break;
					}
					param.nQueryDateType = js_query_date_type.asInt();

					Json::Value js_begin_date = request._param["BeginDate"];
					if (js_begin_date.isNull() || !js_begin_date.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BeginDate\" error";
						break;
					}
					param.nBeginDate = js_begin_date.asInt64();

					Json::Value js_end_date = request._param["EndDate"];
					if (js_end_date.isNull() || !js_end_date.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"EndDate\" error";
						break;
					}
					param.nEndDate = js_end_date.asInt64();

					Json::Value js_begin_row = request._param["BeginRow"];
					if (js_begin_row.isNull() || !js_begin_row.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BeginRow\" error";
						break;
					}
					param.nBeginRow = js_begin_row.asInt();

					Json::Value js_end_row = request._param["EndRow"];
					if (js_end_row.isNull() || !js_end_row.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"EndRow\" error";
						break;
					}
					param.nEndRow = js_end_row.asInt();

					std::string sid;
					int ret = ptr_trade->request_custm_trade_report_limit_order(param, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_LIMITREVOKE:
				{
					dzlog_info("\"ON_LIMITREVOKE\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					LimitRevokeParam param;

					Json::Value js_limit_order_id = request._param["LimitOrderID"];
					if (js_limit_order_id.isNull() || !js_limit_order_id.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"LimitOrderID\" error";
						break;
					}
					param.nLimitOrderID = js_limit_order_id.asInt64();

					Json::Value js_commodity_id = request._param["CommodityID"];
					if (js_commodity_id.isNull() || !js_commodity_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"CommodityID\" error";
						break;
					}
					param.nCommodityID = js_commodity_id.asInt();

					Json::Value js_limit_type = request._param["LimitType"];
					if (js_limit_type.isNull() || !js_limit_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"LimitType\" error";
						break;
					}
					param.nLimitType = js_limit_type.asInt();


					std::string sid;
					int ret = ptr_trade->request_limit_revoke(param, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_MONEYQUERY:
				{
					std::string sid;
					int ret = ptr_trade->request_money_query(sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_MONEYINOUT:
				{
					dzlog_info("\"ON_MONEYINOUT\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					//rettmp = temp->OnMoneyQuery();

					MoneyInOutParam param;

					Json::Value js_operate_type = request._param["OperateType"];
					if (js_operate_type.isNull() || !js_operate_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OperateType\" error";
						break;
					}
					param.OperateType = js_operate_type.asInt();

					Json::Value js_currency = request._param["Currency"];
					if (js_currency.isNull() || !js_currency.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Currency\" error";
						break;
					}
					param.Currency = js_currency.asInt();


					Json::Value js_amount = request._param["Amount"];
					if (js_amount.isNull() || !js_amount.isDouble())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Amount\" error";
						break;
					}
					param.Amount = js_amount.asDouble();


					Json::Value js_fund_psw = request._param["FundPsw"];
					if (js_fund_psw.isNull() || !js_fund_psw.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"FundPsw\" error";
						break;
					}
					int fund_psw_len = js_fund_psw.asString().length();
					if (fund_psw_len > MAX_FUNDPWD_LEN - 1)
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"FundPsw\" length error";
						break;
					}
					strcpy(param.FundPsw, js_fund_psw.asString().c_str());

					Json::Value js_bank_psw = request._param["BankPsw"];
					if (js_bank_psw.isNull() || !js_bank_psw.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BankPsw\" error";
						break;
					}
					int bank_psw_len = js_bank_psw.asString().length();
					if (bank_psw_len > MAX_BANKPWD_LEN - 1)
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BankPsw\" length error";
						break;
					}
					strcpy(param.BankPsw, js_bank_psw.asString().c_str());

					Json::Value js_reversed = request._param["Reversed"];
					if (!js_reversed.isNull() && js_reversed.isString())
					{
						int reversed_len = js_reversed.asString().length();
						if (reversed_len > MAX_IP_LEN - 1)
						{
							js_response["Success"] = 0;
							js_response["Finish"] = 1;
							js_response["ErrStr"] = "\"Reversed\" length error";
							break;
						}
						strcpy(param.Reversed, js_reversed.asString().c_str());
					}

					Json::Value js_pay_type = request._param["PayType"];
					if (js_pay_type.isNull() || !js_pay_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"PayType\" error";
						break;
					}
					param.PayType = js_pay_type.asInt();

					Json::Value js_operate_flag = request._param["OperateFlag"];
					if (js_operate_flag.isNull() || !js_operate_flag.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OperateFlag\" error";
						break;
					}
					param.OperateFlag = js_operate_flag.asInt();

					std::string sid;
					int ret = ptr_trade->request_money_in_out(param, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_LOGINPWDUPD: //修改登录密码
				{
					Json::Value js_old_passwd = request._param["OldPasswd"];
					if (js_old_passwd.isNull() || !js_old_passwd.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OldPasswd\" error";
						break;
					}
					std::string old_passwd = js_old_passwd.asString();

					Json::Value js_new_passwd = request._param["NewPasswd"];
					if (js_new_passwd.isNull() || !js_new_passwd.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"NewPasswd\" error";
						break;
					}
					std::string new_passwd = js_new_passwd.asString();



					Json::Value js_market_type = request._param["MarketType"];
					if (js_market_type.isNull() || !js_market_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"MarketType\" error";
						break;
					}
					int market_type = js_market_type.asInt();

					std::string sid;
					int ret = ptr_trade->login_pwd_upd(old_passwd.c_str(), new_passwd.c_str(), market_type, sid);
					if (0 == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_FUNDPWDUPD: //修改资金密码
				{
					dzlog_info("\"ON_FUNDPWDUPD\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					Json::Value js_old_passwd = request._param["OldPasswd"];
					if (js_old_passwd.isNull() || !js_old_passwd.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OldPasswd\" error";
						break;
					}
					std::string old_passwd = js_old_passwd.asString();

					Json::Value js_new_passwd = request._param["NewPasswd"];
					if (js_new_passwd.isNull() || !js_new_passwd.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"NewPasswd\" error";
						break;
					}
					std::string new_passwd = js_new_passwd.asString();


					std::string sid;
					int ret = ptr_trade->fund_pwd_upd(old_passwd.c_str(), new_passwd.c_str(), sid);
					if (0 == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;

					return_value = true;
					break;
				}
				case ON_FUNDFLOWQUERY: //
				{
					dzlog_info("\"ON_FUNDFLOWQUERY\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					FundFlowQueryParam param;
					Json::Value js_query_type = request._param["QueryType"];
					if (js_query_type.isNull() || !js_query_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"QueryType\" error";
						break;
					}
					param.nQueryType = js_query_type.asInt();

					Json::Value js_begin_date = request._param["BeginDate"];
					if (js_begin_date.isNull() || !js_begin_date.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BeginDate\" error";
						break;
					}
					param.nBeginDate = js_begin_date.asInt64();

					Json::Value js_end_date = request._param["EndDate"];
					if (js_end_date.isNull() || !js_end_date.isInt64())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"EndDate\" error";
						break;
					}
					param.nEndDate = js_end_date.asInt64();

					Json::Value js_begin_row = request._param["BeginRow"];
					if (js_begin_row.isNull() || !js_begin_row.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BeginRow\" error";
						break;
					}
					param.nBeginRow = js_begin_row.asInt();

					Json::Value js_end_row = request._param["EndRow"];
					if (js_end_row.isNull() || !js_end_row.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"EndRow\" error";
						break;
					}
					param.nEndRow = js_end_row.asInt();

					Json::Value js_oper_type = request._param["OperType"];
					if (js_oper_type.isNull() || !js_oper_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"OperType\" error";
						break;
					}
					param.nOperType = js_oper_type.asInt();

					std::string sid;
					int ret = ptr_trade->request_customer_self_fund_flow_query(param, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_SIGNRESULTNOTIFYQUERY:
				{
					dzlog_info("\"ON_SIGNRESULTNOTIFYQUERY\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					SignResultNotifyQueryParam param;
					Json::Value js_bank_id = request._param["BankID"];
					if (js_bank_id.isNull() || !js_bank_id.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BankID\" error";
						break;
					}
					param.BankID = js_bank_id.asInt();

					Json::Value js_bank_account = request._param["BankAccount"];
					if (js_bank_account.isNull() || !js_bank_account.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BankAccount\" error";
						break;
					}
					int bank_account_len = js_bank_account.asString().length();
					if (bank_account_len > MAX_BANKACCNT_LEN - 1)
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"BankAccount\" length error";
						break;
					}
					strcpy(param.BankAccount, js_bank_account.asString().c_str());

					std::string sid;
					int ret = ptr_trade->request_sign_result_notify_query(param, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_PAYFORWARD:
				{
					dzlog_info("\"ON_PAYFORWARD\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					PayForwardParam param;
					Json::Value js_ticket = request._param["Ticket"];
					if (js_ticket.isNull() || !js_ticket.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Ticket\" error";
						break;
					}
					int ticket_len = js_ticket.asString().length();
					if (ticket_len > MAX_TICKET_LEN - 1)
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Ticket\" length error";
						break;
					}
					strcpy(param.Ticket, js_ticket.asString().c_str());

					Json::Value js_identify_code = request._param["IdentifyCode"];
					if (js_identify_code.isNull() || !js_identify_code.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"IdentifyCode\" error";
						break;
					}
					int identify_code_len = js_identify_code.asString().length();
					if (identify_code_len > MAX_IDENTIFYCODE_LEN - 1)
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"IdentifyCode\" length error";
						break;
					}
					strcpy(param.IdentifyCode, js_identify_code.asString().c_str());

					Json::Value js_reversed = request._param["Reversed"];
					if (!js_reversed.isNull() && js_reversed.isString())
					{
						int reversed_len = js_reversed.asString().length();
						if (reversed_len > MAX_IP_LEN - 1)
						{
							js_response["Success"] = 0;
							js_response["Finish"] = 1;
							js_response["ErrStr"] = "\"Reversed\" length error";
							break;
						}

						strcpy(param.Reversed, js_reversed.asString().c_str());
					}

					std::string sid;
					int ret = ptr_trade->request_pay_forward(param, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case ON_CUSTOMERALLOWLOGIN:
				{
					dzlog_info("\"ON_CUSTOMERALLOWLOGIN\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					AllowLoginParam param;
					Json::Value js_type = request._param["Type"];
					if (js_type.isNull() || !js_type.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Type\" error";
						break;
					}
					param.Type = js_type.asInt();

					Json::Value js_allow_login = request._param["AllowLogin"];
					if (js_allow_login.isNull() || !js_allow_login.isInt())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"AllowLogin\" error";
						break;
					}
					param.AllowLogin = js_allow_login.asInt();

					std::string sid;
					int ret = ptr_trade->request_customer_allow_login(param, sid);
					if (Order_Ret_Success == ret)
					{
						js_response["Success"] = 1;
						js_response["Sid"] = sid;
					}
					else
					{
						js_response["Success"] = 0;
					}

					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				case request_type_get_response:
				{
					dzlog_info("\"request_type_get_response\", type = %s, user name = %s", request._type.c_str(), request._account.c_str());
					Json::Value js_sid = request._param["Sid"];
					if (js_sid.isNull() || !js_sid.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Sid\" error";
						break;
					}

					Json::Value js_request_type = request._param["RequestType"];
					if (js_request_type.isNull() || !js_request_type.isString())
					{
						js_response["Success"] = 0;
						js_response["Finish"] = 1;
						js_response["ErrStr"] = "\"Sid\" error";
						break;
					}

					std::string response;
					if (true == ptr_trade->get_callback_response(js_request_type.asString(), js_sid.asString(), js_response))
					{
						js_response["Success"] = 1;
					}
					else
					{
						js_response["Success"] = 0;
					}
					js_response["Finish"] = 1;
					return_value = true;
					break;
				}
				default:
				{
					dzlog_info("[TE] unkown type: %d", type);
					break;
				}
				}
			}
		}
		
	}
	catch (...)
	{	
		js_response["Success"] = 0;
		js_response["Finish"] = 1;
		js_response["ErrStr"] = "exception";
	}

	return return_value;
}

//-------------------------------------------------------------------------------------------
proc_thread_container& g_proc_thread_container()
{
	static proc_thread_container data;
	return data;
}

proc_thread* proc_thread_container::get_thread(pthread_t id)
{
	dzlog_info("proc_thread_container::get_thread, thread id = %ld, map size = %d", id, (int)_container.size());
	proc_thread* ret = NULL;

	std::map<pthread_t, proc_thread*>::iterator iter = _container.find(id);
	if (_container.end() != iter)
	{
		ret = iter->second;
	}
	return ret;
}

bool proc_thread_container::add_thread()
{
	try
	{
		proc_thread* ptr_new = new proc_thread;
		if (false == ptr_new->init())
		{
			dzlog_info("thread init error\n");
			delete ptr_new;
			return false;
		}
		else
		{
			g_proc_thread_select().update_thread_user_count(ptr_new->get_pthread_id());
			_container.insert(std::pair<pthread_t, proc_thread*>(ptr_new->get_pthread_id(), ptr_new));
			return true;
		}


	}
	catch (const bad_alloc& e)
	{
		dzlog_info("thread molloc error\n");
		return false;
	}
}
void proc_thread_container::destory()
{
	std::map<pthread_t, proc_thread*>::iterator iter = _container.begin();
	std::map<pthread_t, proc_thread*>::iterator end = _container.end();
	for (; iter != end; ++iter)
	{
		iter->second->destory();
		delete iter->second;
	}
	_container.clear();
}
