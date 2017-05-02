#ifndef APPLOG_H
#define APPLOG_H
#include <stdio.h>
#include <string>

#include "TradeCommon.h"

///应用日志类
class SDKDLL_API AppLog {
public:
	AppLog();
	~AppLog();

public:
///****************************************
/// @brief 用于初始化日志。在main函数里只需调用一次。
/// @param  strLogPath      :   日志路径
/// @param  nFile       :   文件日志开关  0 -- off   1 -- on
/// @param  nTerm       : 打屏日志开关  0 -- off   1 -- on
/// @return 返回处理结果状态 
/// - 0 : 初始化成功
/// - !0 : 失败
///****************************************
    int InitLog(std::string strLogPath, int nFile, int nTerm);

	
///****************************************
/// @brief 用于刷新日志文件
/// @return  无
///****************************************
	void FlushLog();

///****************************************
/// @brief 用于关闭日志句柄，释放资源。在main函数里只需调用一次。
/// @return 返回处理结果状态 
/// - 0 : 成功
/// - !0 : 失败
///****************************************
	int ReleaseLog();

};

#endif //APPLOG_H
