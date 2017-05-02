#ifndef _QUOTE_DATA_DEF_H_
#define _QUOTE_DATA_DEF_H_
#include <string>
#include <vector>

using std::string;
using std::vector;

/** 行情数据结构定义
*/
struct QuoteMsg
{
	QuoteMsg():
		_type(0),
		_bit(0),
		_timestamp(0),
		_tradedate(0)
	{

	}
	int _type;
	string _exchange;
	string _market;
	string _symbol;
	string _currency;
	int _bit;
	string _open;
	string _close;
	string _ask;
	string _bid;
	string _last;
	string _high;
	string _low;
	int _timestamp;
	int _tradedate;
	string _quoteOrg;
};

//行情快照
typedef vector<QuoteMsg> QuoteSnapshot;

/** k线种类
*/
enum KLINE_TYPE
{
	KLINE_TYPE_1MIN = 0,
	KLINE_TYPE_5MIN,
	KLINE_TYPE_15MIN,
	KLINE_TYPE_30MIN,
	KLINE_TYPE_60MIN,
	KLINE_TYPE_1DAY,
};


/** 分时线种类
*/
enum ShareLineType
{
	SHARE_TYPE_1MIN = 0
};

/** 产品信息
*/
class ProdcutInfo
{
public:
	ProdcutInfo()
	{
	}
	ProdcutInfo(const string& productId, const string& tableName, const string& topicName) :
		_productId(productId),
		_tableName(tableName),
		_topicName(topicName)
	{

	}
	
	//产品ID
	string _productId;

	//数据库表名称
	string _tableName;

	//mqtt topicName
	string _topicName;
};

//产品信息参数
typedef vector<ProdcutInfo> ProductInfoParm;

//产品列表
typedef vector<string> ProductList;


/** k线数据
*/
class KLINE_DATA
{
public:
	KLINE_DATA():
        _stockTime(0)
    {
    }

	int _stockTime;
	string _fmtTime;
	string _open;
	string _close;
	string _high;
	string _low;
};

//k线数据列表
typedef vector<KLINE_DATA> KLineDataList;

//k线种类接口定义
#define KLINE_TYPE_TEXT_1MIN	"1min"
#define KLINE_TYPE_TEXT_5MIN	"5min"
#define KLINE_TYPE_TEXT_15MIN	"15min"
#define KLINE_TYPE_TEXT_30MIN	"30min"
#define KLINE_TYPE_TEXT_60MIN	"60min"
#define KLINE_TYPE_TEXT_1DAY	"1day"


//k线回调接口
class IKLineCallBack 
{
public:
	//mqtt 发布消息
	virtual void publishMsg(const string& topicName, const string& msg) = 0;

	//执行数据库操作
	virtual bool ExecuteDb(const string& str) = 0;
};

#endif
