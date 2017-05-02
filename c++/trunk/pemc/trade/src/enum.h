#ifndef _ENUM_H_39DDBB76_9675_433A_9527_2378A1D02DD9_
#define _ENUM_H_39DDBB76_9675_433A_9527_2378A1D02DD9_




enum login_status
{
	login_status_in, //��¼״̬
	login_status_out,//�ǵ�¼״̬
};

enum request_type
{
	request_type_get_response = 10000,

};



//���׽ӿ�����
enum {
	LOGIN = 0,		//��¼
	LOGOUT = 98,	//�ǳ�
	GET_ACCNTINFO = 2,	//��ȡ�û���Ϣ
	GET_MARKETSTATUS = 3,	//��ȡ�г�״̬

	GET_OPENMARKETCONF = 5,	//������ƷID����ȡ���м۽��ֵ�������Ϣ
	GET_CLOSEMARKETCONF = 6, //������ƷID����ȡ���м�ƽ�ֵ�������Ϣ
	GET_OPENLIMITCONF = 7,	//������ƷID����ȡ���޼۽��ֵ�������Ϣ
	GET_CLOSELIMITCONF = 8,	//������ƷID����ȡ���޼�ƽ�ֵ�������Ϣ
	GET_OPENDELIVERYCONF = 50,	//������ƷID����ȡ�����������Ϣ

	REQ_QUOTE = 9,			//������Ʒ����
	REQ_HOLDPOSITION = 10,	//����ֲ���Ϣ
	REQ_LIMITORDERS = 11,	//�����޼۵���Ϣ
	REQ_CLOSEORDERS = 12,	//����ƽ�ֵ���Ϣ
	REQ_HOLDPOSITIONTOTAL = 13, //�������гֲֵ���Ϣ

	QUE_QUOTE_ID = 21,
	QUE_HOLDPOSITION_ID = 22,
	QUE_LIMITORDERS_ID = 23,
	QUE_CLOSEORDERS_ID = 24,
	QUE_HOLDPOSITIONTOTAL_ID = 25,
	QUE_HOLDPOSITIONTOTAL_DIREC = 26,

	ON_OPENMARKET = 14,	//�м۽���
	ON_CLOSEMARKET = 31,	//�м�ƽ��
	ON_CLOSEMARETMANY = 16, //����ƽ��
	ON_OPENLIMIT = 19,	//�޼۽���
	ON_CLOSELIMIT = 35, //�޼�ƽ��
	ON_LIMITREVOKE = 36,
	ON_MONEYQUERY = 17,	//
	ON_MONEYINOUT = 18, //�����
	ON_CUSTMTRADEREPORTHOLDPOSITOIN = 37,
	ON_CUSTMTRADEREPORTCLOSEPOSITOIN = 38,
	ON_CUSTMTRADEREPORTLIMITORDER = 39,
	ON_LOGINPWDUPD = 40,	//�޸�����
	ON_FUNDPWDUPD = 41,		//�޸�����
	ON_FUNDFLOWQUERY = 42,
	ON_OPENDELIVERYORDER = 51,
	ON_SIGNRESULTNOTIFYQUERY = 46,
	ON_PAYFORWARD = 47,
	ON_CUSTOMERALLOWLOGIN = 48,

	EXIT = 99,	//�˳�
};

#endif