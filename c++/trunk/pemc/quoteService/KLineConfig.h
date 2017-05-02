#ifndef _KLINE_CONFIG_H_
#define _KLINE_CONFIG_H_

#include "QuoteDataDef.h"

/**  k线配置管理类
*/
class KLineConfig 
{
public:

	//获取默认缓存大小
	static int getBufSize(const KLINE_TYPE& kLineType);
	static int getBufSize(const ShareLineType& shareLineType);

	//获取K线产品参数
	static void getProductInfoParm(const KLINE_TYPE& kLineType, ProductInfoParm& productInfoParm);

	//获取分时线线产品参数
	static void getProductInfoParm(const ShareLineType& shareLineType, ProductInfoParm& productInfoParm);

	//获取分钟线标志
	static int getMinFlag(const KLINE_TYPE& kLineType);

	//获取topic名字
	static string getMqttTopicName(const string& productId, const KLINE_TYPE& kLineType);

	//获取数据库表的名字
	static string getTableName(const string& productId, const KLINE_TYPE& kLineType);

    //获取时间间隔
    static int getIntervalDay(const KLINE_TYPE& kLineType);

    //获取产品列表

    static const ProductList& getProductList();
};
#endif