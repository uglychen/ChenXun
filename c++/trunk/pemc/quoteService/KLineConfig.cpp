#include "KLineConfig.h"
#include "Product_def.h"

//配置信息结构
struct ConfigInfo
{
	string _productId;
	string _topicName;

};



int KLineConfig::getBufSize(const KLINE_TYPE& kLineType)
{
	switch (kLineType)
	{
	case KLINE_TYPE_1MIN:
		return 24 * 60;
		break;
	case KLINE_TYPE_5MIN:
		return 24 * 60 / 5;
		break;
	case KLINE_TYPE_15MIN:
		return 24 * 60 / 15;
		break;
	case KLINE_TYPE_30MIN:
		return 24 * 60 / 30;
		break;
	case KLINE_TYPE_60MIN:
		return 24;
		break;
	case KLINE_TYPE_1DAY:
		return 1;
		break;
	}

	return 1;
}

int KLineConfig::getBufSize(const ShareLineType& shareLineType)
{
	switch (shareLineType)
	{
	case SHARE_TYPE_1MIN:
		return 24 * 60;
		break;
	default:
		break;
	}
	return 1;
}

void KLineConfig::getProductInfoParm(const KLINE_TYPE& kLineType, ProductInfoParm& productInfoParm)
{
	productInfoParm.clear();

	switch (kLineType)
	{
	case KLINE_TYPE_1MIN:
		break;
	case KLINE_TYPE_5MIN:
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_SLIVER, DB_KLINE_SLIVER_5MIN, TOPIC_KLINE_SLIVER_5MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PALLADIUM, DB_KLINE_PALLADIUM_5MIN, TOPIC_KLINE_PALLADIUM_5MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PLATINUM, DB_KLINE_PLATINUM_5MIN, TOPIC_KLINE_PLATINUM_5MIN));
		break;
	case KLINE_TYPE_15MIN:
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_SLIVER, DB_KLINE_SLIVER_15MIN, TOPIC_KLINE_SLIVER_15MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PALLADIUM, DB_KLINE_PALLADIUM_15MIN, TOPIC_KLINE_PALLADIUM_15MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PLATINUM, DB_KLINE_PLATINUM_15MIN, TOPIC_KLINE_PLATINUM_15MIN));
		break;
	case KLINE_TYPE_30MIN:
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_SLIVER, DB_KLINE_SLIVER_30MIN, TOPIC_KLINE_SLIVER_30MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PALLADIUM, DB_KLINE_PALLADIUM_30MIN, TOPIC_KLINE_PALLADIUM_30MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PLATINUM, DB_KLINE_PLATINUM_30MIN, TOPIC_KLINE_PLATINUM_30MIN));
		break;
	case KLINE_TYPE_60MIN:
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_SLIVER, DB_KLINE_SLIVER_60MIN, TOPIC_KLINE_SLIVER_60MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PALLADIUM, DB_KLINE_PALLADIUM_60MIN, TOPIC_KLINE_PALLADIUM_60MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PLATINUM, DB_KLINE_PLATINUM_60MIN, TOPIC_KLINE_PLATINUM_60MIN));
		break;
	case KLINE_TYPE_1DAY:
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_SLIVER, DB_KLINE_SLIVER_1DAY, TOPIC_KLINE_SLIVER_1DAY));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PALLADIUM, DB_KLINE_PALLADIUM_1DAY, TOPIC_KLINE_PALLADIUM_1DAY));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PLATINUM, DB_KLINE_PLATINUM_1DAY, TOPIC_KLINE_PLATINUM_1DAY));
		break;
	default:
		break;
	}

}

void KLineConfig::getProductInfoParm(const ShareLineType& shareLineType, ProductInfoParm& productInfoParm)
{
	productInfoParm.clear();

	switch (shareLineType)
	{
	case SHARE_TYPE_1MIN:
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_SLIVER, DB_SHARE_LINE_SLIVER_1MIN, TOPIC_SHARE_ONE_SLIVER_1MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PALLADIUM, DB_SHARE_LINE_PALLADIUM_1MIN, TOPIC_SHARE_PALLADIUM_1MIN));
		productInfoParm.push_back(ProdcutInfo(PRODUCT_ID_PLATINUM, DB_SHARE_LINE_PLATINUM_1MIN, TOPIC_SHARE_PLATINUM));
		break;
	default:
		break;
	}
}

int KLineConfig::getMinFlag(const KLINE_TYPE& kLineType)
{
	switch (kLineType)
	{
	case KLINE_TYPE_1MIN:
		return 1;
		break;
	case KLINE_TYPE_5MIN:
		return 5;
		break;
	case KLINE_TYPE_15MIN:
		return 15;
		break;
	case KLINE_TYPE_30MIN:
		return 30;
		break;
	default:
		break;
	}

	return 1;
}

string KLineConfig::getMqttTopicName(const string& productId, const KLINE_TYPE& kLineType)
{

	ProductInfoParm productInfolist;
	getProductInfoParm(kLineType, productInfolist);

	for (const ProdcutInfo& value: productInfolist)
	{
		if (value._productId == productId)
		{
			return value._topicName;
		}
	}

	return "";
}

string KLineConfig::getTableName(const string& productId, const KLINE_TYPE& kLineType)
{
	ProductInfoParm productInfolist;
	getProductInfoParm(kLineType, productInfolist);

	for (const ProdcutInfo& value : productInfolist)
	{
		if (value._productId == productId)
		{
			return value._tableName;
		}
	}

	return "";
}

int KLineConfig::getIntervalDay(const KLINE_TYPE& kLineType)
{
    return 5;
}

const ProductList& KLineConfig::getProductList()
{
    static ProductList productlist = { PRODUCT_ID_SLIVER , PRODUCT_ID_PALLADIUM , PRODUCT_ID_PLATINUM };
    return productlist;
}
