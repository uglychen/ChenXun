#ifndef APPLOG_H
#define APPLOG_H
#include <stdio.h>
#include <string>

#include "TradeCommon.h"

///Ӧ����־��
class SDKDLL_API AppLog {
public:
	AppLog();
	~AppLog();

public:
///****************************************
/// @brief ���ڳ�ʼ����־����main������ֻ�����һ�Ρ�
/// @param  strLogPath      :   ��־·��
/// @param  nFile       :   �ļ���־����  0 -- off   1 -- on
/// @param  nTerm       : ������־����  0 -- off   1 -- on
/// @return ���ش�����״̬ 
/// - 0 : ��ʼ���ɹ�
/// - !0 : ʧ��
///****************************************
    int InitLog(std::string strLogPath, int nFile, int nTerm);

	
///****************************************
/// @brief ����ˢ����־�ļ�
/// @return  ��
///****************************************
	void FlushLog();

///****************************************
/// @brief ���ڹر���־������ͷ���Դ����main������ֻ�����һ�Ρ�
/// @return ���ش�����״̬ 
/// - 0 : �ɹ�
/// - !0 : ʧ��
///****************************************
	int ReleaseLog();

};

#endif //APPLOG_H
