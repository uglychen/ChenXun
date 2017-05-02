#ifndef __TRADE_EVENT_CODE_H__
#define __TRADE_EVENT_CODE_H__

//��Ϣ״̬
enum
{
	MSG_STATUS_NO_UPDATE      = -5001,		///< ��Ϣδ����
	MSG_STATUS_UPDATED        = 0,			///< ��Ϣ����
	MSG_STATUS_NO_PERMISSION  = -5003,		///< ��Ϣ�޷���Ȩ
	MSG_STATUS_NOT_EXIST      = -5004,    ///< ��Ϣ������
	MSG_STATUS_INIT_FAILED    = -5005,    ///< ��Ϣ����ʧ��
};

//�ӿ�״̬(��������)
enum
{
	RET_WARN_LIFESTAGE_ERR	  = -7999,  ///< ���������쳣
	RET_WARN_NOT_LOGIN		  = -7888,  ///< �û�δ��¼
};

//�ӿ�״̬(��������)
enum
{
	RET_FATAL_NETWORK_ERR	  = -99998,     ///< �����쳣
	RET_FATAL_ACCESS_ERR	  = -99993,     ///< ����ʧ��
	RET_FATAL_DROPDOWN		  = -89999,		///< ����
	RET_FATAL_APPCHECKOUT	  = -79995,		///< ��֤ʧ��
	RET_FATAL_FIRSTLOGIN	  = -79994,     ///< �״ε�¼
};


//���׼��
enum
{
	RET_SUCCEED				  = 0,	///<  ����ɹ�

	RET_ERR_QUANTITY          = -4901,	///< ����������
	RET_ERR_SLPRICE			  = -4902,	///< ֹ�������
	RET_ERR_TPPRICE			  = -4903,	///< ֹӯ������
	RET_ERR_OPENDIECTOR		  = -4904,	///< ���ַ�������
	RET_ERR_CLOSEDIRECTOR	  = -4905,	///< ƽ�ַ�������
	RET_ERR_TRADERANGE		  = -4906,	///< ��Χ����
	RET_ERR_OPERATETYPE		  = -4907,	///< ������������
	RET_ERR_INOUTAMOUNT		  = -4908,	///< ����������
	RET_ERR_COMMODITYRIGHT	  = -4909,	///< ��ƷȨ�޲���
	RET_ERR_COMMODITYID		  = -4910,	///< ��Ʒ������
	RET_ERR_NOHOLDPOSITION    = -4911,	///< ���޳ֲ�
	RET_ERR_DATERANGE         = -4912,  ///< ���ڷ�Χ����
	RET_ERR_TIMERANGE         = -4913,  ///< ʱ�䷶Χ����
	RET_ERR_QUERYTYPE         = -4914,  ///< ��ѯ��������
	RET_ERR_BANKSIGNINFO	  = -4915,  ///< ����ǩԼ��Ϣ����

	RET_ERR_RESPONETIMEOUT	  = -4888,	///< ��ѯ���س�ʱ
	RET_ERR_STAMP             = -4777,	///< ���Ƹ�Ƶ����
};


enum
{
	EVENT_CODE_LOGIN               =   1,	///< ��¼�¼�
	EVENT_CODE_QUOTE               =   2,	///< ���������¼�
	EVENT_CODE_INITIALIZE          =   3,	///< ��ʼ���¼�

	EVENT_CODE_OPENMARKETORDER     =   4,	///< �м۽����¼�
	EVENT_CODE_CLOSEMARKETORDER    =   5,	///< �м�ƽ���¼�
	EVENT_CODE_OPENLIMITORDER      =   6,	///< �޼۽����¼�
	EVENT_CODE_LIMITCLOSEPOSITION  =   7,	///< �޼�ƽ���¼�
	EVENT_CODE_LIMITREVOKE         =   8,	///< �����޼��¼�
	EVENT_CODE_SYSBULLETIN         =   9,	///< ϵͳ�����¼�
	EVENT_CODE_DISCONNECT          =  10,	///< �����¼�
	EVENT_CODE_SYSBUL_LIMITCLOSE   =  11,	///< �޼۳ɽ��¼�
	EVENT_CODE_SYSBUL_LIQUIDATION  =  12,	///< ն���¼�
	EVENT_CODE_MONEYINOUT		   =  13,			///< ������¼�
	EVENT_CODE_MONEYQUERY		   =  14,			///< �����ʽ��ѯ�¼�
	EVENT_CODE_HEARTBEAT            =  15,	///< �����¼�
	EVENT_CODE_CUSTMTRADEREPORTHOLDPOSITIONQUERY =16,	///< �ֲֵ������ѯ�¼�
	EVENT_CODE_CUSTMTRADEREPORTCLOSEPOSITIONQUERY =17,	///< ƽ�ֵ������ѯ�¼�
	EVENT_CODE_CUSTMTRADEREPORTLIMITORDERQUERY =18,	///< �޼۵������ѯ�¼�
	EVENT_CODE_PWDMODIFY          =  19,	///< �����޸��¼�
	EVENT_CODE_CUSTOMERSELFFUNDFLOWQUERY =20,	///< ������ѯ�¼�
	EVENT_CODE_OPENDELIVERYORDER    =  21,  ///< ���������¼�
	EVENT_CODE_SIGNRESULTNOTIFYQUERY	= 22,	///< ǩԼ���֪ͨ�¼�
	EVENT_CODE_PAYFORWARD				= 23,	///< ֧���ƽ��¼�
	EVENT_CODE_CUSTOMERALLOWLOGIN		= 24,	///< �޸Ļ��ѯ�ͻ��Ƿ���Ե�¼
};

#endif
