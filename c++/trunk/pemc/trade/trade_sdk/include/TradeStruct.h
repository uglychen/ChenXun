#ifndef __TRADE_STRUCT_H__
#define __TRADE_STRUCT_H__


#define  MAX_CUSTOMERNAME_LEN             128
#define  MAX_COMMODITYNAME_LEN            128
#define  MAX_COMMODITYCODE_LEN            16
#define  MAX_COMMODITYTYPE_LEN            16
#define  MAX_LOGINACCONT_LEN              32
#define  MAX_MESSAGE_LEN                  512

#define  MAX_BULLETIN_HEADER_LEN          128
#define  MAX_BULLETIN_CONTENT_LEN         4000
#define  MAX_PUBLISHERNAME_LEN            64

#define  MAX_BANKPWD_LEN				  128
#define  MAX_FUNDPWD_LEN				  128
//#define  MAX_LOGINPWD_LEN                 128
#define  MAX_BANKACCNT_LEN				  32
#define  MAX_BANKNAME_LEN				  64
#define  MAX_CERTNUMBER_LEN               32
#define  MAX_SPECIFICATIONUNIT_LEN        16
#define  MAX_OP_LOGIN_ACCOUNT_LEN         64
#define  MAX_APPDES_LEN                      128
#define  MAX_MONEYINOUT_MSG_LEN           128
#define  MAX_MONEYINOUT_SID_LEN           72
#define  MAX_MONEYINOUT_URL_LEN           264
#define  MAX_DELICOMM_LEN                 5
#define  MAX_STRSID_LEN                   32
#define  MAX_TICKET_LEN					  256
#define  MAX_IDENTIFYCODE_LEN				  16
#define  MAX_IP_LEN						  128
#define  MAX_FUNDFLOW_REMARK_LEN		  64
/// 登录用户信息结构体
typedef struct _USER_INFO
{
	int  LoginID;		///< 登录ID
	char LoginName[MAX_LOGINACCONT_LEN];	///< 用户名
	int  UserID;		///< 用户ID
	int  MemberID;		///< 会员ID
	int  CustomerID;	///< 客户ID
	int  UserType;		///< 用户类型
	char CustomerName[MAX_COMMODITYNAME_LEN];	///< 客户名称
}UserInfo;


/// 实时行情结构体
typedef struct _REALTIEM_QUOTE
{
    int       CommodityID;	///< 商品ID
    double    SellPrice;	///< 卖出价
    double    BuyPrice;		///< 买入价
    double    HighPrice;	///< 最高价
    double    LowPrice;		///< 最低价
    long long QuoteTime;	///< 报价时间
}RealTimeQuote;



/// 账户信息结构体
typedef struct _ACCOUNT_INFO
{
    char   Account[MAX_LOGINACCONT_LEN];	///< 账户名
    char   LoginAccount[MAX_LOGINACCONT_LEN];	///< 登录名
	char   CustomerName[MAX_CUSTOMERNAME_LEN];	///< 客户名称
	double NAVPrice;            ///< 净值
    double Amount;              ///< 余额
	double OpenProfit;          ///< 浮动盈亏
	double ExchangeReserve;     ///< 交易准备金
	double PerformanceReserve;  ///< 履约准备金
	double FrozenReserve;       ///< 冻结准备金
	double RiskRate;            ///< 风险率
}AccountInfo;


/// 客户银行信息
typedef struct _CUSTOMERBANKINFO
{
	int		BankID;								///< 银行ID
	char	BankAccount[MAX_BANKACCNT_LEN];		///< 银行卡号
	char	BankName[MAX_BANKNAME_LEN];			///< 银行名称
	char    FundPwd[MAX_FUNDPWD_LEN];			///< 资金密码
	int		SignStatus;							///< 签约状态
	int		CertificateType;					///< 证件类型
	char	CertificateNum[MAX_CERTNUMBER_LEN];	///< 证件号码
}CustomerBankInfo;


/// 交易所余额信息
typedef struct _BOURSEMONEY_INFO
{
	double Amount;				///< 交易所余额
	double AmountAvailable;		///< 交易所可用资金
	double AmountFetchable;		///< 交易所可提取资金
}BourseMoneyInfo;


/// 商品信息结构体
typedef struct _COMMODITY_INFO
{
    int       CommodityID;	///< 商品ID
    char      CommodityName[MAX_COMMODITYNAME_LEN];		///< 商品名称
	long long CommodityRight;	///< 商品权限
	long long TradeRight;	///< 交易权限
	double    AgreeUnit;	///< 合约单位
	int       Currency;		///< 货币种类
	double    MinQuoteChangeUnit;	///< 最小行情变动单位
	double    MinPriceUnit;	///< 最小价格单位
	double    FixedSpread;	///< 固定点差
    double    BuyPrice;		///< 买入价
    double    SellPrice;	///< 卖出价
    double    HighPrice;	///< 最高价
    double    LowPrice;		///< 最低价
    long long QuoteTime;	///< 报价时间
	int       CommodityClass;   ///< 商品类型
	char      CommodityClassName[MAX_COMMODITYNAME_LEN];	///< 商品类型名称
	int       CommodityMode;	///< 处理方式 0-旧商品 1-新商品 2-特殊商品
	int       IsDisplay;    ///< 是否展示
	int       CommodityGroupID;	///< 商品组ID
	char      CommodityGroupName[MAX_COMMODITYNAME_LEN];	///< 商品组名称
	double    WeightStep;	///< 重量步进值
	double    WeightRadio;	///< 重量换算
    int       TradeType;    ///< 交易类型
    double    SpecificationRate;  ///< 规格系数
    char      SpecificationUnit[MAX_SPECIFICATIONUNIT_LEN];  ///< 规格单位
}CommodityInfo;


/// 持仓单信息结构体
typedef struct _HOLDPOSITION_INFO
{
    long long HoldPositionID;	///< 持仓单ID
	int       CommodityID;		///< 商品ID
	char      CommodityName[MAX_COMMODITYNAME_LEN];	///< 商品名称
	int       OpenType;			///< 建仓类型  --- ::1:客户下单  --- 4:限价下单
	int       OpenDirector;		///< 建仓方向
	int       Quantity;			///< 持仓数量
	double	  TotalWeight;		///< 持仓总重量
	double    OpenPrice;		///< 建仓价格
	double    HoldPositionPrice;	///< 持仓价
	double    ClosePrice;		///< 平仓价
	long long SLLimitOrderID;	///< 止损单ID
	double    SLPrice;			///< 止损价
	long long TPLimitOrderID;	///< 止盈单ID
	double    TPPrice;			///< 止盈价
	double    OpenProfit;		///< 浮动盈亏
	double    CommissionAmount;	///< 手续费
	long long OpenDate;			///< 建仓时间
	double    AgreeMargin;		///< 履约保证金
	double    Freezemargin;		///< 冻结保证金
	double    OverdueFindFund;	///< 滞纳金
}HoldPositionInfo;


/// 限价单信息结构体
typedef struct _LIMITORDER_INFO
{
	long long LimitOrderID;		///< 限价单ID
	int       CommodityID;		///< 商品ID
	char      CommodityName[MAX_COMMODITYNAME_LEN];		///< 商品名称
	int       LimitType;		///< 限价单类型	--- ::1:限价建仓 ---2:止盈平仓  ---3:止损平仓
	int       OrderType;		///< 建仓类型  --- ::1.客户下单
	int       OpenDirector;		///< 建仓方向
	double    OrderPrice;		///< 建仓价
	double    SLPrice;			///< 止损价
	double    TPPrice;			///< 止盈价
	int       OpenQuantity;		///< 持仓数量
	double	  TotalWeight;		///< 持仓总重量
	long long CreateDate;		///< 建仓时间
	long long ExpireType;		///< 失效类型
	long long UpdateDate;		///< 更新时间
	double    FreeszMargin;		///< 冻结保证金
	int		  DealStatus;		///< 处理状态  ---1:限价单未成交 ---2:限价单已成交 ---3:限价单用户撤销 ---4:成交撤单 ---5:市价平仓撤销 ---6:斩仓撤销 ---7:限价平仓撤销 ---8:结算撤单 ---9:异常撤销 ---10:交割撤销
}LimitOrderInfo;


/// 平仓单信息结构体
typedef struct _CLOSEPOSITION_INFO
{
	long long ClosePositionID;	///< 平仓单ID
	int       CommodityID;		///< 商品ID
	char      CommodityName[MAX_COMMODITYNAME_LEN];	///< 商品名称
	int       CloseDirector;	///< 平仓方向
	double    OpenPrice;		///< 建仓价
	double    HoldPrice;		///< 持仓价
	double    ClosePrice;		///< 平仓价
	int       Quantity;			///< 持仓数量
	double	  TotalWeight;		///< 持仓总重量
	long long OpenPositionID;	///< 建仓单ID
	double    CommissionAmount;	///< 手续费
	long long OpenDate;			///< 建仓时间
	long long CloseDate;		///< 平仓时间
	int       MemberID;			///< 会员ID
	int       OpenType;			///< 建仓类型  --- ::1:客户下单  --- 4:限价下单
	int       CloseType;		///< 平仓类型  --- ::1:客户下单  --- 4:限价下单  --- 5:斩仓强平
}ClosePositionInfo;


/// 持仓汇总单信息结构体
typedef struct _HOLDPOSITION_TOTAL_INFO
{
    int    CommodityID;			///< 商品ID
	char   CommodityName[MAX_COMMODITYNAME_LEN];	///<  商品名称
	int    OpenDirector;		///< 建仓方向
	int    Quantity;			///< 持仓数量
	double TotalWeight;			///< 持仓总重量
	double OpenPriceTotal;		///< 建仓总值
	double AvgOpenPrice;		///< 建仓均价
	double HoldPriceTotal;		///< 持仓总值
	double AvgHoldPrice;		///< 持仓均价
	double ClosePrice;			///< 平仓价
	double OpenProfit;			///< 浮动盈亏

}HoldPositionTotalInfo;


/// 限价撤销单信息结构体
typedef struct _LIMITREVOKE_INFO
{
	long long LimitOrderID;		///< 限价单ID
	int       LimitType;		///< 限价单类型
}LimitRevokeInfo;


/// 限价平仓单临时结构体
typedef struct _LIMITCLOSE_INFO
{
	long long HoldPositionID;   ///< 持仓单ID
	long long SLLimitOrderID;   ///< 止损单ID
	double    SLPrice;          ///< 止损价格
	long long TPLimitOrderID;   ///< 止盈单ID
	double    TPPrice;          ///< 止盈价格
	long long ExpireType;       ///< 失效类型
}LimitCloseInfo;


/// 交割单信息结构体
typedef struct  _DELIVERYORDER_INFO
{
	long long  DeliveryOrderID;		///< 交割单ID
	int  CommodityID;				///< 商品ID
	//int  DeliveryCommodityID;		///< 交割商品ID
	char  DeliveryCommodityName[MAX_COMMODITYNAME_LEN];  ///< 交割商品名称
	long long  HoldpositionID;		///< 持仓单ID
	int  Quantity;					///<  交割数量
	double TotalWeight;				///< 交割总重量
	double DeliveryPrice;			///<  交割价
	long long  DeliveryDate;		///< 交割时间
	long long  DeliveryDeadline;	///< 交割期限
	int DeliveryStatus;				///< 交割状态
	long long  ApplyDate;			///<  申请时间
	double Commission;				///< 手续费
	double Amount;					///<  交割货款
	int  DeliveryType;				///<  交割类型   0:买方交割
}DeliveryOrderInfo;


///客户交易报表持仓单信息结构体
typedef struct _CUSTMTRADEREPORTHOLDPOSITION_INFO
{
	long long  tradedate;		///< 交易日
	long long  holdpositionid;	///< 持仓单号
	long long  opendate;		///< 建仓时间
	int        commodityid;		///< 商品标示
	char       commoditycode[MAX_COMMODITYCODE_LEN];	///< 商品编号
	char       commodityname[MAX_COMMODITYNAME_LEN];	///< 商品名称
	int        holdquantity;	///< 持仓数量
	int        opendirector;	///< 建仓方向 
	double     openprice;		///< 建仓价格
	double     holdpositionpric;	///< 持仓价格
	double     slprice;			///< 止损价
	double     tpprice;			///< 止盈价
	double     settlementpl;	///< 结算盈亏
	double     commission;		///< 手续费
	double     latefee;			///< 滞纳金
	double     perfmargin;		///< 履约保证金
	double     settleprice;		///< 结算价
	int        openquantity;	///< 建仓数量
	double     holdweight;		///< 持仓总重量
	double     openweight;		///< 建仓总重量
	int        commoditymode;	///< 商品类型
}CustmTradeReportHoldPositionInfo;

///客户交易报表平仓单信息结构体
typedef struct _CUSTMTRADEREPORTCLOSEPOSITION_INFO
{
	long long	tradedate;		///< 交易日
	long long	holdpositionid;	///< 持仓单号
	long long	opendate;		///< 建仓时间
	int			commodityid;	///< 商品标示
	char		commoditycode[MAX_COMMODITYCODE_LEN];	///< 商品编号
	char		commodityname[MAX_COMMODITYNAME_LEN];	///< 商品名称
	int			closequantity;	///< 平仓数量
	int			opendirector;	///< 建仓方向
	double		openprice;		///< 建仓价格
	double		holdpositionpric;	///< 持仓价格
	long long	closepositionid;	///< 平仓单号
	long long	closedate;		///< 平仓时间
	int			closedirector;	///< 平仓方向
	double		closeprice;		///< 平仓价格
	double		commission;		///< 手续费
	double		profitorloss;	///< 盈亏
	double		opencommission;	///< 建仓手续费
	double		closeweight;	///< 平仓总重量
	int			commoditymode;	///< 商品类型
}CustmTradeReportClosePositionInfo;

///客户交易报表限价单信息结构体
typedef struct _CUSTMTRADEREPORTLIMITORDER_INFO
{
	long long	tradedate;		///< 交易日
	long long	limitorderid;	///< 限价单号
	long long	createdate;		///< 下单时间
	int			commodityid;	///< 商品标示
	char		commoditycode[MAX_COMMODITYCODE_LEN];	///< 商品编号
	char		commodityname[MAX_COMMODITYNAME_LEN];	///< 商品名称
	int			openquantity;	///< 建仓数量
	int			opendirector;	///< 建仓方向
	int			limittype;		///< 类型
	double		orderprice;		///< 限价
	double		tpprice;		///< 止损价
	double		slprice;		///< 止盈价
	int			deadline;		///< 期限
	double		frozenreserve;	///< 冻结保证金
	double		openweight;		///< 减仓总重量
	int			commoditymode;	///< 商品模式
}CustmTradeReportLimitOrderInfo;


/// 客户端配置信息结构体
typedef struct _CLIENTCONFIG
{
	int     TradeMode;			///< 交易模式
	int     MarketParamType;	///< 参数类型
	double  MarketParamValue;	///< 参数取值
}ClientConfig;


/// 商品费项信息结构体
typedef struct _COMMODITYFEEGROUP
{
	int CommodityID;			///< 商品标识
	int CommodityFeeID;			///< 费用标识
	int ChargeType;				///< 计提方式
	int CollectType;			///< 收取方式
	int FeeType;				///< 费用父类
	int FeeSubType;				///< 费用子类
	double  Value;				///< 取值
	int EffectStatus;			///< 生效状态
	int IsDelivery;				///< 是否交割费用
}CommodityFeeGroup;

/// 处理结果结构体
typedef struct _PROCESSRESULT
{
	int  RetCode;				///< 返回码
	char Message[MAX_MESSAGE_LEN];		///< 返回信息
	char StrSid[MAX_STRSID_LEN + 1];    ///< SID
}ProcessResult;

///修改或查询客户是可以登陆
typedef struct _ALLOWLOGINPARAM
{
	int Type;							///< 请求类型	  --- 1:修改   --- 2:查询
	int AllowLogin;                     ///< 是否允许登陆 --- 0:不允许 --- 1:允许
}AllowLoginParam;

/// 修改或查询客户是可以登陆
typedef struct _ALLOWLOGININFO
{
	int RetCode;						///< 返回码
	int AllowLogin;						///< 是否允许登陆 --- 0:不允许 --- 1:允许
	char Message[MAX_MESSAGE_LEN];      ///< 返回信息
}AllowLoginInfo;

/// 公告结构体
typedef struct _SYSBULLETIN_INFO
{
	int       LoginID;		///< 登录ID
	long long BulletinID;   ///< 公告ID
	int       TradeMode;		///< 交易模式
	int       BulletinType;		///< 公告类型
	int       BulletinPriorty;  ///< 优先级
	char      BulletinHeader[MAX_BULLETIN_HEADER_LEN];		///< 公告消息头
	char      BulletinContent[MAX_BULLETIN_CONTENT_LEN];		///< 公告消息正文
	char      PublisherName[MAX_PUBLISHERNAME_LEN];		///< 发布方名
	int       BulletinMethod;		///< 公告方式
}SysBulletinInfo;

///出入金请求返回信息
typedef struct _MONEYINOUT_INFO
{
	int		  RetCode;    ///< 返回码
	char	  StrSid[MAX_STRSID_LEN + 1];    ///< SID
	char	  LocalSid[MAX_MONEYINOUT_SID_LEN];  ///< 新增LocalSid
	char	  BankSid[MAX_MONEYINOUT_SID_LEN];   ///< 新增BankSid
	char	  NotifyUrl[MAX_MONEYINOUT_URL_LEN]; ///< 新增NotifyUrl
	char	  Message[MAX_MESSAGE_LEN];			 ///< 出入金请求返回信息
	char	  Ticket[MAX_TICKET_LEN];			 ///< 
}MoneyInOutInfo;

typedef struct _DELIVERYCOMMODITY
{
	int   CommodityID;	///< 交割商品ID
	char  CommodityName[MAX_COMMODITYNAME_LEN];	///< 交割商品名称
	int   TradecommodityClassID;	///< 交割商品类型
	double  DeliveryRadio;      ///< 交割手续费比例
}DeliveryCommodity;

///  市价单建仓配置信息结构体
typedef struct _OPENMARKETORDER_CONF
{
	int       CommodityID;		///< 商品ID
    double    MinQuantity;		///< 每单最大交易手数
    double    MaxQuantity;		///< 每单最小交易手数
	double    MinTradeRange;	///< 市价单允许点差的最小值
	double    MaxTradeRange;	///< 市价单允许点差的最大值
	double    DefaultTradeRange;	///< 市价单允许点差的起始默认值
	double    AgreeUnit;		///< 合约单位
	int       CommodityMode;	///< 处理方式 0-旧商品 1-新商品 2-特殊商品
	double    WeightStep;		///< 重量步进值
	double    WeightRadio;		///< 重量换算
	double    MinTotalWeight;	///< 每单最小交易总重
	double	  MaxTotalWeight;	///< 每单最大交易总重
	double    DepositeRate;		///< 准备金率
	double    SpecificationRate;	///< 规格系数
}OpenMarketOrderConf;



/// 限价单建仓配置信息结构体
typedef struct _OPENLIMITORDER_CONF
{
    int       CommodityID;		///< 商品ID
    double    MinQuantity;		///< 每单最小交易手数
    double    MaxQuantity;		///< 每单最大交易手数
	double    LimitSpread;		///< 限价点差
	double    FixSpread;		///< 固定点差
	double    TPSpread;			///< 止盈点差
	double    SLSpread;			///< 止损点差
	double    MinPriceUnit;		///< 最小价格单位
	double    AgreeUnit;		///< 合约单位
	int       CommodityMode;	///< 处理方式 0-旧商品 1-新商品 2-特殊商品
	double    WeightStep;		///< 重量步进值
	double    WeightRadio;		///< 重量换算
	double    MinTotalWeight;   ///< 每单最小交易总重
	double    MaxTotalWeight;   ///< 每单最大交易总重
	double    DepositeRate;     ///< 准备金率
	double    SpecificationRate;    ///< 规格系数
}OpenLimitOrderConf;

/// 市价单平仓配置信息结构体
typedef struct _CLOSEMARKETORDER_CONF
{
	int       CommodityID;		///< 商品ID
	double    MinQuantity;		///< 每单最小交易手数
	double    MaxQuantity;		///< 每单最大交易手数
	double    MinTradeRange;	///< 市价单允许点差的最小值
	double    MaxTradeRange;	///< 市价单允许点差的最大值
	double    DefaultTradeRange;	///< 市价单允许点差的起始默认值
	double    AgreeUnit;		///< 合约单位
	int       CommodityMode;	///< 处理方式 0-旧商品 1-新商品 2-特殊商品
	double    WeightStep;		///< 重量步进值
	double    WeightRadio;		///< 重量换算
	double    MinTotalWeight;   ///< 每单最小交易总重
	double    MaxTotalWeight;   ///< 每单最大交易总重
	double    DepositeRate;     ///< 准备金率
	double    SpecificationRate;    ///< 规格系数
}CloseMarketOrderConf;


/// 限价单平仓配置信息结构体
typedef struct _LIMITCLOSEPOSITION_CONF
{
	int       CommodityID;		///< 商品ID
	double    FixedSpread;		///< 固定点差
	double    TPSpread;			///< 止盈点差
	double    SLSpread;			///< 止损点差
	double    MinPriceUnit;		///< 最小价格单位
	double    AgreeUnit;		///< 合约单位
	int       CommodityMode;	///< 处理方式 0-旧商品 1-新商品 2-特殊商品
	double    WeightRadio;		///< 重量换算
}LimitClosePositionConf;

/// 交割申请配置信息结构体
typedef struct _OPENDELIVERYORDER_CONF
{
	int  CommodityID;		///< 交易商品ID
	int  CanDeliver;		///< 是否能交割		---0:不能交割		---1-能交割
	double  AgreeUnit;		///< 合约单位
	double  DeliveryCharge;	///<  交割货款
	struct  _DELIVERYCOMMODITY DeliveryCommodity[MAX_DELICOMM_LEN];		///< 交易商品对应的交割商品信息
	int  DeliCommCnt;		///<  对应交割商品总数 [0,n)
}OpenDeliveryOderConf;

/// 市价单建仓 
typedef struct _OPENMARKETORDER_PARAM
{
	int nCommodityID;		///< 商品ID
	int nOpenDirector;		///< 建仓方向		--- ::OPENDIRECTOR_BUY:买  --- OPENDIRECTOR_SELL:卖
	double dbPrice;			///< 建仓单价
	double dbWeight;		///< 交易重量(kg)
	int nQuantity;			///< 交易数量
	double dbTradeRange;	///< 最大点差
	//int nOrderType;			///< 下单类型		---1:客户下单
}OpenMarketOrderParam;

/// 第一次登陆后的密码设置参数
/*typedef struct _PWDSET_PARAM
{
	char LoginPwd[MAX_LOGINPWD_LEN];			///< 登陆密码
	char FundPwd[MAX_FUNDPWD_LEN];			///< 资金密码
	char TelPwd[MAX_TELPWD_LEN];			///< 电话密码
	char TradePwd[MAX_TRADEPWD_LEN];			///< 交易密码
}PwdSetParam;*/

/// 市价平仓单入参结构体 
typedef struct _CLOSEMARKETORDER_PARAM
{
	long long nHoldPositionID;		///< 持仓ID
	int nCommodityID;				///< 商品ID
	double dbWeight;				///< 交易重量(kg)
	int nQuantity;					///< 平仓数量
	int nTradeRange;				///< 最大点差
	double dbPrice;					///< 平仓价格
	//int nClosePositionType;			///< 平仓类型		--- 1:客户下单
}CloseMarketOrderParam;


/// 限价单建仓入参结构体 
typedef struct _OPENLIMITODER_PARAM
{
	int nCommodityID;			///< 商品ID
	int nExpireType;			///< 过期类型		--- 1:当日有效
	int nOpenDirector;			///< 建仓方向		--- ::OPENDIRECTOR_BUY:买  --- OPENDIRECTOR_SELL:卖
	double dbWeight;			///< 交易重量(kg)
	int nQuantity;				///< 建仓数量
	//int nOrderType;				///< 下单类型		--- 1:客户下单
	double dbOrderPrice;		///< 建仓单价
	double dbTPPrice;			///< 止盈价格
	double dbSLPrice;			///< 止损价格
}OpenLimitOrderParam;


/// 限价单平仓入参结构体
typedef struct _CLOSELIMITORDER_PARAM
{
	int nCommodityID;			///< 商品ID
	double dbClosePrice;		///< 平仓单价
	int nExpireType;			///< 过期类型		--- 1:当日有效
	long long nHoldPositionID;	///< 持仓ID
	//int nOrderType;				///< 下单类型		--- 1:客户下单
	double dbSLPrice;			///< 止损价格
	double dbTPPrice;			///< 止盈价格
}CloseLimitOrderParam;

///客户出入金查询入参结构体
typedef struct _FUNDFLOWQUERY_PARAM
{
	int nQueryType;				///< 请求类型 (目前只能查询历史报表)  --- 1:当前报表  --- 2:历史报表
	long long nBeginDate;       ///< 查询范围的起始时间(UTC秒数)
	long long nEndDate;         ///< 查询范围的终止时间(UTC秒数)
	int nBeginRow;              ///< 开始记录序号 --- 1~n:第一条记录 --- -1:全部 --- 0:无记录
	int nEndRow;                ///< 结束记录序号 --- 1~n:第n条记录 --- -1:全部 --- 0:无记录
	int nOperType;				///< 操作类型 --- 0:出入金 --- 1:入金 --- 2:出金
}FundFlowQueryParam;

///客户出入金信息结构体
typedef struct _FUNDFLOWQUERY_INFO
{
	int     FlowNumber;         ///< 流水号
	int     OperType;           ///< 操作类型 --- 15:入金 --- 16:出金 --- 17:入金冲正 --- 18:出金冲正 --- 19:撤销入金 --- 20:撤销出金 --- 21:单边账调整入金 --- 22:单边账调整出金 --- 23:穿仓回退入金 --- 24:穿仓回退出金
	double  BeforeAmount;       ///< 起始金额
	double  Amount;             ///< 变动金额
	double  AfterAmount;        ///< 变后金额
	char    OpLoginAccount[MAX_OP_LOGIN_ACCOUNT_LEN];   ///< 操作员
	long long   OperDate;       ///< 日期
	int     BankID;             ///< 银行标识
	int		OrderID;			///< 关联单号
	char	Remark[MAX_FUNDFLOW_REMARK_LEN];			///< 红包标识
}FundFlowQueryInfo;

///客户交易报表持仓/平仓/限价单查询入参结构体
typedef struct _REPORTQUERY_PARAM
{
	int nQueryDateType;			///< 请求类型 (目前只能查询历史报表)  --- 1:当前报表  --- 2:历史报表
	long long nBeginDate;		///< 查询范围的起始时间(UTC秒数)
	long long nEndDate;			///< 查询范围的终止时间(UTC秒数)
	int nBeginRow;				///< 开始记录序号 --- 1~n:第一条记录 --- -1:全部 --- 0:无记录
	int nEndRow;				///< 结束记录序号 --- 1~n:第n条记录 --- -1:全部 --- 0:无记录
}ReportQueryParam;

/// 限价单撤销入参结构体
typedef struct _LIMITREVOKE_PARAM
{
	long long nLimitOrderID;	///< 限价单ID
	int nCommodityID;			///< 商品ID
	//int nOrderType;				///< 下单类型		--- 1:客户下单
	int nLimitType;				///< 限价单类型		--- ::LIMITTYPE_OPENLIMIT:建仓  --- LIMITTYPE_TP_CLOSE:止盈  --- LIMITTYPE_SL_CLOSE:止损
}LimitRevokeParam;


/// 批量平仓入参结构体 
typedef struct _CLOSEMARKETORDERMANY_PARAM
{
	int nCommodityID;			///< 商品ID
	double dbWeight;			///< 交易重量(kg)
	int nQuantity;				///< 平仓数量
	int nTradeRange;			///< 最大点差
	double dbPrice;				///< 平仓价格
	//int nClosePositionType;		///< 平仓类型		--- 1:一般平仓
	int nCloseDirector;			///< 平仓方向		--- ::OPENDIRECTOR_BUY:买  --- OPENDIRECTOR_SELL:卖
}CloseMarketOrderManyParam;

/// 出入金入参结构体
typedef struct	_MONEYINOUT_PARAM
{
	int OperateType;			///< 出入金类型		--- ::MONEY_OUT:出金  --- MONEY_IN:入金
	int Currency;				///< 币种		--- 1:人民币
	double Amount;				///< 出入金数量
	char FundPsw[MAX_FUNDPWD_LEN];	///< 资金密码
	char BankPsw[MAX_BANKPWD_LEN];	///< 银行密码
	char Reversed[MAX_IP_LEN];		///< 特殊会员用于传入IP
	int PayType;                    ///< 支付类型 仅用于新浪支付 ---0:普通出入金 ---1:余额支付 ---:2绑卡支付
	int OperateFlag;                ///< 操作标志 仅用于新浪支付 ---0:普通出入金 ---1:红包入金
}MoneyInOutParam;

/// 支付推进入参结构体
typedef struct  _PAYFORWARD_PARAM
{
	char Ticket[MAX_TICKET_LEN];	///< Ticket  
	char IdentifyCode[MAX_IDENTIFYCODE_LEN];	///< 验证码
	char Reversed[MAX_IP_LEN];      ///< 特殊会员用于传入IP
}PayForwardParam;


/// 签约成功结果通知入参结构体
typedef struct _SIGNRESULTNOTIFYQUERY_PARAM
{
	int     BankID;                             ///< 银行ID --- ::TradeDefine.h BANK_xxx
	char    BankAccount[MAX_BANKACCNT_LEN];     ///< 新签约账号 
}SignResultNotifyQueryParam;


/// 交割申请入参结构体
typedef struct  _OPENDELIVERYORDER_PARAM
{
	int nCommodityID;   ///<   商品ID
	long long nHoldPositionID;    ///<   持仓单ID
	int nDeliveryCommodityID;   ///<   交割商品ID
	char sAppDesc[MAX_APPDES_LEN];   ///<  申请备注
// int  DeliveryType;    ///<  交割类型
}OpenDeliveryOrderParam;

typedef struct _REQHOLDINFO_INFO
{
	char OperateSid[MAX_STRSID_LEN + 1];
	long long OperateTime;
}ReqHoldInfo;

#endif //__STRUCT_H__
