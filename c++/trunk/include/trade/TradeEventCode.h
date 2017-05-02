#ifndef __TRADE_EVENT_CODE_H__
#define __TRADE_EVENT_CODE_H__

//信息状态
enum
{
	MSG_STATUS_NO_UPDATE      = -5001,		///< 信息未更新
	MSG_STATUS_UPDATED        = 0,			///< 信息正常
	MSG_STATUS_NO_PERMISSION  = -5003,		///< 信息无访问权
	MSG_STATUS_NOT_EXIST      = -5004,    ///< 信息不存在
	MSG_STATUS_INIT_FAILED    = -5005,    ///< 信息更新失败
};

//接口状态(甭重连类)
enum
{
	RET_WARN_LIFESTAGE_ERR	  = -7999,  ///< 生命周期异常
	RET_WARN_NOT_LOGIN		  = -7888,  ///< 用户未登录
};

//接口状态(需重连类)
enum
{
	RET_FATAL_NETWORK_ERR	  = -99998,     ///< 网络异常
	RET_FATAL_ACCESS_ERR	  = -99993,     ///< 建立失败
	RET_FATAL_DROPDOWN		  = -89999,		///< 断连
	RET_FATAL_APPCHECKOUT	  = -79995,		///< 认证失败
	RET_FATAL_FIRSTLOGIN	  = -79994,     ///< 首次登录
};


//交易检查
enum
{
	RET_SUCCEED				  = 0,	///<  处理成功

	RET_ERR_QUANTITY          = -4901,	///< 总重量有误
	RET_ERR_SLPRICE			  = -4902,	///< 止损价有误
	RET_ERR_TPPRICE			  = -4903,	///< 止盈价有误
	RET_ERR_OPENDIECTOR		  = -4904,	///< 建仓方向有误
	RET_ERR_CLOSEDIRECTOR	  = -4905,	///< 平仓方向有误
	RET_ERR_TRADERANGE		  = -4906,	///< 点差范围有误
	RET_ERR_OPERATETYPE		  = -4907,	///< 操作类型有误
	RET_ERR_INOUTAMOUNT		  = -4908,	///< 出入金额有误
	RET_ERR_COMMODITYRIGHT	  = -4909,	///< 商品权限不足
	RET_ERR_COMMODITYID		  = -4910,	///< 商品不存在
	RET_ERR_NOHOLDPOSITION    = -4911,	///< 并无持仓
	RET_ERR_DATERANGE         = -4912,  ///< 日期范围有误
	RET_ERR_TIMERANGE         = -4913,  ///< 时间范围有误
	RET_ERR_QUERYTYPE         = -4914,  ///< 查询类型有误
	RET_ERR_BANKSIGNINFO	  = -4915,  ///< 银行签约信息有误

	RET_ERR_RESPONETIMEOUT	  = -4888,	///< 查询返回超时
	RET_ERR_STAMP             = -4777,	///< 限制高频操作
};


enum
{
	EVENT_CODE_LOGIN               =   1,	///< 登录事件
	EVENT_CODE_QUOTE               =   2,	///< 行情推送事件
	EVENT_CODE_INITIALIZE          =   3,	///< 初始化事件

	EVENT_CODE_OPENMARKETORDER     =   4,	///< 市价建仓事件
	EVENT_CODE_CLOSEMARKETORDER    =   5,	///< 市价平仓事件
	EVENT_CODE_OPENLIMITORDER      =   6,	///< 限价建仓事件
	EVENT_CODE_LIMITCLOSEPOSITION  =   7,	///< 限价平仓事件
	EVENT_CODE_LIMITREVOKE         =   8,	///< 撤销限价事件
	EVENT_CODE_SYSBULLETIN         =   9,	///< 系统公告事件
	EVENT_CODE_DISCONNECT          =  10,	///< 断连事件
	EVENT_CODE_SYSBUL_LIMITCLOSE   =  11,	///< 限价成交事件
	EVENT_CODE_SYSBUL_LIQUIDATION  =  12,	///< 斩仓事件
	EVENT_CODE_MONEYINOUT		   =  13,			///< 出入金事件
	EVENT_CODE_MONEYQUERY		   =  14,			///< 可用资金查询事件
	EVENT_CODE_HEARTBEAT            =  15,	///< 心跳事件
	EVENT_CODE_CUSTMTRADEREPORTHOLDPOSITIONQUERY =16,	///< 持仓单报表查询事件
	EVENT_CODE_CUSTMTRADEREPORTCLOSEPOSITIONQUERY =17,	///< 平仓单报表查询事件
	EVENT_CODE_CUSTMTRADEREPORTLIMITORDERQUERY =18,	///< 限价单报表查询事件
	EVENT_CODE_PWDMODIFY          =  19,	///< 密码修改事件
	EVENT_CODE_CUSTOMERSELFFUNDFLOWQUERY =20,	///< 出入金查询事件
	EVENT_CODE_OPENDELIVERYORDER    =  21,  ///< 交割申请事件
	EVENT_CODE_SIGNRESULTNOTIFYQUERY	= 22,	///< 签约结果通知事件
	EVENT_CODE_PAYFORWARD				= 23,	///< 支付推进事件
	EVENT_CODE_CUSTOMERALLOWLOGIN		= 24,	///< 修改或查询客户是否可以登录
};

#endif
