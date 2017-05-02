#ifndef __TRADE_UNIT_H__
#define __TRADE_UNIT_H__

/// 规整小数位
double formatprice(double value, double minunit);

/// 手数转重量
/// <param name="qty">重量</param>
/// <param name="agreeUnit">合约单位</param>
/// <param name="weightRadio">重量换算</param>
/// <param name="specificateRate">规格系数</param>
double QuantityToWeightDouble(int qty, double agreeUnit, double weightRadio, double specificateRate);

/// 重量转手数
/// <param name="weight">重量</param>
/// <param name="agreeUnit">合约单位</param>
/// <param name="weightRadio">重量转换系数</param>
/// <param name="specificateRate">规格系数</param>
int WeightToQuantityInt(double weight, double agreeUnit, double weightRadio, double specificateRate);

/// 参考可下单重量
/// <param name="exchangeReserve">交易准备金</param>
/// <param name="price">建仓价</param>
/// <param name="depositeRate">准备金率</param>
/// <param name="weightRadio">重量换算</param>
/// <param name="specificateRate">规格系数</param>
double ReferenceTradeWeightDouble(double exchangeReserve, double price, double depositeRate, double weightRadio, double specificateRate);

/// 参考可下单手数
/// <param name="exchangeReserve">交易准备金</param>
/// <param name="price">建仓价</param>
/// <param name="agreeUnit">合约单位</param>
/// <param name="depositeRate">准备金率</param>
double ReferenceTradeQtyDouble(double exchangeReserve, double  price, double agreeUnit, double depositeRate);

/// 履约准备金参考
/// <param name="price">建仓价</param>
/// <param name="weight">交易重量</param>
/// <param name="weightRadio">重量换算系数</param>
/// <param name="depositeRate">准备金率</param>
/// <param name="specificateRate">规格系数</param>
double ReferencePerformanceReserveDouble(double price, double weight, double weightRadio, double depositeRate, double specificateRate);

/// 冻结准备金参考
/// <param name="price">价格</param>
/// <param name="weight">总重量</param>
/// <param name="weightRadio">重量换算系数</param>
/// <param name="depositeRate">准备金率</param>
/// <param name="specificateRate">规格系数</param>
double ReferenceFreezenReserveDouble(double price, double weight, double weightRadio, double depositeRate, double specificateRate);


#endif //__TRADE_UNIT_H__
