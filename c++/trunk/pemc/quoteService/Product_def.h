#ifndef _PRODUCT_DEF_H_
#define _PRODUCT_DEF_H_

//张凯杰本地测开启
//#define  _TEST_ZHANG_ 1

//阿里云测试开启
#define  _TEST_ALIYUN 1

//Activemq 队列*************************************************************

//broker 名字

#define  BROKER_URI "failover://(tcp://101.37.33.121:61616)"

//原始行情主题名字
#define QUOTE_SINGLE_ORG "Quote.Single.Org"

//***************************************************************************


// 产品信息 ***********************************************************************************
//产品ID_银
#define PRODUCT_ID_SLIVER		"GDAG"
//产品ID_钯
#define PRODUCT_ID_PALLADIUM	"GDPD"
//产品ID_铂
#define PRODUCT_ID_PLATINUM		"GDPT"

//产品名字_银
#define PRODUCT_NAME_SLIVER		"粤贵银"
//产品名字钯
#define PRODUCT_NAME_PALLADIUM	"粤贵钯"
//产品名字_铂
#define PRODUCT_NAME_PLATINUM   "粤贵铂"

//*******************************************************************************************

//MQTT*************************************************************************************



#ifdef _TEST_ALIYUN
const std::string ADDRESS("tcp://101.37.33.121:1883");
#elif _TEST_ZHANG_
const std::string ADDRESS("tcp://172.18.14.213:1883");
#endif

const int  QOS = 1;
const long TIMEOUT = 10000L;

//mqtt publisher clientID
const std::string CLIENTID_QUOTE("AsyncPublisher_Quote_chen");
const std::string CLIENTID_SHARE_1MIN("AsyncPublisher_Share_1min_chen");

const std::string CLIENTID_KLINE_1MIN("AsyncPublisher_KLine_1Min_chen");
const std::string CLIENTID_KLINE_5MIN("AsyncPublisher_KLine_5min_chen");
const std::string CLIENTID_KLINE_15MIN("AsyncPublisher_KLine_15min_chen");
const std::string CLIENTID_KLINE_30MIN("AsyncPublisher_KLine_30min_chen");
const std::string CLIENTID_KLINE_60MIN("AsyncPublisher_KLine_60min_chen");
const std::string CLIENTID_KLINE_1DAY("AsyncPublisher_KLine_1day_chen");

//实时行情
#define TOPIC_QUOTE_SLIVER						"Gg.Midle.Quote.Sliver_chen"
#define TOPIC_QUOTE_PALLADIUM					"Gg.Midle.Quote.Palladium_chen"
#define TOPIC_QUOTE_PLATINUM					"Gg.Midle.Quote.Platinum_chen"

//一分钟分时线
#define TOPIC_SHARE_ONE_SLIVER_1MIN				"Gg.Midle.Share.Sliver.1min_chen"
#define TOPIC_SHARE_PALLADIUM_1MIN				"Gg.Midle.Share.Palladium.1min_chen"
#define TOPIC_SHARE_PLATINUM					"Gg.Midle.Share.Platinum.1min_chen"

//粤贵银
#define TOPIC_KLINE_SLIVER_5MIN					"Gg.Midle.KLine.Sliver.5min"
#define TOPIC_KLINE_SLIVER_15MIN				"Gg.Midle.KLine.Sliver.15min"
#define TOPIC_KLINE_SLIVER_30MIN				"Gg.Midle.KLine.Sliver.30min"
#define TOPIC_KLINE_SLIVER_60MIN				"Gg.Midle.KLine.Sliver.60min"
#define TOPIC_KLINE_SLIVER_1DAY					"Gg.Midle.KLine.Sliver.1day"

//粤贵钯
#define TOPIC_KLINE_PALLADIUM_5MIN			"Gg.Midle.KLine.Palladium.5min"
#define TOPIC_KLINE_PALLADIUM_15MIN			"Gg.Midle.KLine.Palladium.15min"
#define TOPIC_KLINE_PALLADIUM_30MIN			"Gg.Midle.KLine.Palladium.30min"
#define TOPIC_KLINE_PALLADIUM_60MIN			"Gg.Midle.KLine.Palladium.60min"
#define TOPIC_KLINE_PALLADIUM_1DAY			"Gg.Midle.KLine.Palladium.1day"


//粤贵铂
#define TOPIC_KLINE_PLATINUM_5MIN			"Gg.Midle.KLine.Platinum.5min"
#define TOPIC_KLINE_PLATINUM_15MIN			"Gg.Midle.KLine.Platinum.15min"
#define TOPIC_KLINE_PLATINUM_30MIN			"Gg.Midle.KLine.Platinum.30min"
#define TOPIC_KLINE_PLATINUM_60MIN			"Gg.Midle.KLine.Platinum.60min"
#define TOPIC_KLINE_PLATINUM_1DAY			"Gg.Midle.KLine.Platinum.1day"


//*************************************************************************************

//数据库配置***************************************************************************

//数据库是否使用阿里云
#define  DATA_BASE_USE_ALI 1
#ifdef DATA_BASE_USE_ALI
#define DBHOST "tcp://116.62.176.131:3306/calf_middle_gg"
#define USER "user1"
#define PASSWORD "123456"
#else
#define DBHOST "tcp://172.18.14.213:3306/calf_middle_gg"
#define USER "root"
#define PASSWORD "123456"
#endif


//产品K线数据库表名_银_5分钟
#define DB_KLINE_SLIVER_5MIN		"silver_kline_5min"
//产品K线数据库表名_钯_5分钟
#define DB_KLINE_PALLADIUM_5MIN		"palladium_kline_5min"
//产品K线数据库表名_铂_5分钟
#define DB_KLINE_PLATINUM_5MIN		"platinum_kline_5min"

//产品K线数据库表名_银_15分钟
#define DB_KLINE_SLIVER_15MIN		"silver_kline_15min"
//产品K线数据库表名_钯_15分钟
#define DB_KLINE_PALLADIUM_15MIN	"palladium_kline_15min"
//产品K线数据库表名_铂_15分钟
#define DB_KLINE_PLATINUM_15MIN		"platinum_kline_15min"

//产品K线数据库表名_银_30分钟
#define DB_KLINE_SLIVER_30MIN		"silver_kline_30min"
//产品K线数据库表名_钯_30分钟
#define DB_KLINE_PALLADIUM_30MIN	"palladium_kline_30min"
//产品K线数据库表名_铂_30分钟
#define DB_KLINE_PLATINUM_30MIN		"platinum_kline_30min"

//产品K线数据库表名_银_60分钟
#define DB_KLINE_SLIVER_60MIN		"silver_kline_60min"
//产品K线数据库表名_钯_60分钟
#define DB_KLINE_PALLADIUM_60MIN	"palladium_kline_60min"
//产品K线数据库表名_铂_60分钟
#define DB_KLINE_PLATINUM_60MIN		"platinum_kline_60min"

//产品K线数据库表名_银_1日
#define DB_KLINE_SLIVER_1DAY		"silver_kline_1day"
//产品K线数据库表名_钯_1日
#define DB_KLINE_PALLADIUM_1DAY		"palladium_kline_1day"
//产品K线数据库表名_铂_1日
#define DB_KLINE_PLATINUM_1DAY		"platinum_kline_1day"


//分时线数据库表名_银_1分钟
#define DB_SHARE_LINE_SLIVER_1MIN		"silver_share_line_1min"
//分时线数据库表名_钯_1分钟
#define DB_SHARE_LINE_PALLADIUM_1MIN    "palladium_share_line_1min"
//分时线数据库表名_铂_1分钟
#define DB_SHARE_LINE_PLATINUM_1MIN      "platinum_share_line_1min"


//******************************************************************************************************



// CGI ***************************************************************************************************
#define  CGI_REQUEST_QUEUE "Quote.Cgi.Request_chen"

//获取行情快照接口
#define CGI_REQUEST_QUOTE_SNAPSHOT "pemc.mid.quote.snapshot"

//获取K线接口
#define CGI_REQUEST_QUOTE_KLINE "pemc.mid.quote.kLine"

//获取分时线接口
#define CGI_REQUEST_QUOTE_SHARELINE "pemc.mid.quote.shareLine"

//行情时间种类_1分钟
#define QUOTE_TIME_TYPE_1_MIN "1min"

//行情时间种类_5分钟
#define QUOTE_TIME_TYPE_5_MIN "5min"

//行情时间种类_15分钟
#define QUOTE_TIME_TYPE_15_MIN "15min"

//行情时间种类_30分钟
#define QUOTE_TIME_TYPE_30_MIN "30min"

//行情时间种类_60分钟
#define QUOTE_TIME_TYPE_60_MIN "60min"

//行情时间种类_1天
#define QUOTE_TIME_TYPE_1_DAY "1day"

//******************************************************************************************************

#endif