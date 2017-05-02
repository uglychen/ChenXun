#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "TradeClient.h"
#include <map>

typedef  struct _HANDLE_EVNET
{
        string  hLoginacc;
        int             hIndex;
        int             hEvent;
        int             hTimes;
}handle_event;  

enum {
	LOGIN                           =  0,
	LOGOUT                          =  98,
	GET_ACCNTINFO           =  2,
	GET_MARKETSTATUS        =  3,

	GET_OPENMARKETCONF      =  5,
	GET_CLOSEMARKETCONF     =  6,
	GET_OPENLIMITCONF       =  7,
	GET_CLOSELIMITCONF      =  8,
	GET_OPENDELIVERYCONF    =  50,

	REQ_QUOTE                       =  9,
	REQ_HOLDPOSITION        = 10,
	REQ_LIMITORDERS         = 11,
	REQ_CLOSEORDERS         = 12,
	REQ_HOLDPOSITIONTOTAL   = 13,

	QUE_QUOTE_ID            = 21,
	QUE_HOLDPOSITION_ID     = 22,
	QUE_LIMITORDERS_ID      = 23,
	QUE_CLOSEORDERS_ID      = 24,
	QUE_HOLDPOSITIONTOTAL_ID        = 25,
	QUE_HOLDPOSITIONTOTAL_DIREC     = 26,

	ON_OPENMARKET           = 14,
	ON_CLOSEMARKET          = 31,
	ON_CLOSEMARETMANY       = 16,
	ON_OPENLIMIT            = 19,
	ON_CLOSELIMIT           = 35,
	ON_LIMITREVOKE          = 36,
	ON_MONEYQUERY           = 17,
	ON_MONEYINOUT           = 18,
	ON_CUSTMTRADEREPORTHOLDPOSITOIN = 37,
	ON_CUSTMTRADEREPORTCLOSEPOSITOIN = 38,
	ON_CUSTMTRADEREPORTLIMITORDER = 39,
	ON_LOGINPWDUPD          = 40,
	ON_FUNDPWDUPD           = 41,
	ON_FUNDFLOWQUERY        = 42,
	ON_OPENDELIVERYORDER    = 51,
	ON_SIGNRESULTNOTIFYQUERY		= 46,
	ON_PAYFORWARD					= 47,
	ON_CUSTOMERALLOWLOGIN           = 48,

	EXIT                            = 99,
}; 

#define MAX_USER 10 
#define MAX_EVENT 18
#define CONFIG_PATH	"./param.conf"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ssl_mutex = PTHREAD_MUTEX_INITIALIZER;

map<int, TradeClient*> *mClient = new map<int, TradeClient*>;
TradeClient *temp = new TradeClient[MAX_USER];
int mCommodity[] = 
{
	100000,100001,1000002,100000000,100000000,100000001,100000002,100000000,1000000003,100003,100004
};

int countusr = 0;

void usrprocess(handle_event *th_param)
{	
        handle_event *th_handle = (handle_event *)th_param;
        printf("[T] [%s] Now Create newthread for [%s]...\n",getime().c_str(), th_handle->hLoginacc.c_str());
	
		int release_ret = -1;
		int curStat = temp[th_handle->hIndex].GetCurrStatus();
		printf("[T] [%s] usrloginid[%s],curStat[%d], online[%ld]\n",getime().c_str(), th_handle->hLoginacc.c_str(), curStat, mClient->size());
		if(-99 < curStat && curStat < 0)
		{
			printf ("[T] [%s] Now Rlease Interface : %s, stat: %d\n",getime().c_str(), th_handle->hLoginacc.c_str(), curStat);
			pthread_mutex_lock(&mutex );
			mClient->erase(th_handle->hIndex);
			pthread_mutex_unlock(&mutex);
			pthread_mutex_lock(&ssl_mutex);
			release_ret = temp[th_handle->hIndex].Release();
			if(release_ret == 0)
				countusr -= 1;
			pthread_mutex_unlock(&ssl_mutex);
			return;
		}
														
        if(-99 == temp[th_handle->hIndex].GetCurrStatus())
		{
                printf("[T] [%s] Now Create TradeClient for [%s]...\n",getime().c_str(), th_handle->hLoginacc.c_str());

                int ret = -1;

				pthread_mutex_lock(&ssl_mutex);
                ret = temp[th_handle->hIndex].Init();
				pthread_mutex_unlock(&ssl_mutex);
                if(0 != ret){
                        printf("[TE] Init fail. TODO Release!\n");
						pthread_mutex_lock(&ssl_mutex);
                        temp[th_handle->hIndex].Release();
						pthread_mutex_unlock(&ssl_mutex);
                        return ;
                }
                std::string acc(th_handle->hLoginacc);
                std::string pwd("111111");
                ret = temp[th_handle->hIndex].Login(th_handle->hLoginacc,"111111");
				countusr += 1;
				if(0 != ret){
                        printf( "[TE] Login fail. TODO Release!\n");
						pthread_mutex_lock(&ssl_mutex);
						release_ret = temp[th_handle->hIndex].Release();
						if(release_ret == 0)
							countusr -= 1;
						pthread_mutex_unlock(&ssl_mutex);
						return ;
                }
                pthread_mutex_lock(&mutex);
				mClient->insert(pair<int, TradeClient*>(th_handle->hIndex, temp+th_handle->hIndex));
                pthread_mutex_unlock(&mutex);
                printf("[T]  [%s] finish! acc: %s\n",getime().c_str(), th_handle->hLoginacc.c_str());
        }

		if(1)
        {
                int rettmp = -1;
                srand((int)time(0));
        		int nID = rand()%42;
        		th_handle->hEvent = nID;
                int mCommodityID = mCommodity[3];
				sleep(3);
				switch(th_handle->hEvent){
                case GET_ACCNTINFO:
                        {
                            rettmp = temp[th_handle->hIndex].GetAccountInfo();
                            break;
                        }
                case GET_MARKETSTATUS:
                            {
                                rettmp = temp[th_handle->hIndex].GetMarketStatus();
                                break;
                            }
                case GET_OPENMARKETCONF:
                            {
                                OpenMarketOrderConf mOpenMarketOrderConf;
                                rettmp = temp[th_handle->hIndex].GetOpenMarketOrderConf(mCommodityID,mOpenMarketOrderConf);
                                break;
                            }
                case GET_CLOSEMARKETCONF:
                            {
                                CloseMarketOrderConf mCloseMarketOrderConf;
                                rettmp = temp[th_handle->hIndex].GetCloseMarketOrderConf(mCommodityID,mCloseMarketOrderConf);
                                break;
                            }
                case GET_OPENLIMITCONF:
                            {
                                OpenLimitOrderConf mOpenLimitOrderConf;
                                rettmp = temp[th_handle->hIndex].GetOpenLimitOrderConf(mCommodityID,mOpenLimitOrderConf);
                                break;
                            }
                case GET_CLOSELIMITCONF:
                            {
                                LimitClosePositionConf mLimitClosePositionConf;
                                rettmp = temp[th_handle->hIndex].GetCloseLimitOrderConf(mCommodityID,mLimitClosePositionConf);
                                break;
                            }
				case GET_OPENDELIVERYCONF:
							{
								OpenDeliveryOderConf mOpenDeliveryOderConf;
								double mHoldWeight = 2;
								double mHoldPrice = 3000;
								rettmp = temp[th_handle->hIndex].GetOpenDeliveryOrderConf(mCommodityID,mHoldWeight,mHoldPrice,mOpenDeliveryOderConf);
								break;
							}
                case REQ_QUOTE:
                            {
                                rettmp = temp[th_handle->hIndex].RequestQuote();
                                break;
                            }
                case REQ_HOLDPOSITION:
                            {
                                rettmp = temp[th_handle->hIndex].RequestHoldPosition();
                                break;
                            }
                case REQ_LIMITORDERS:
							{
								rettmp = temp[th_handle->hIndex].RequestLimitOrders();
								break;
							}
				case REQ_CLOSEORDERS:
							{
								rettmp = temp[th_handle->hIndex].RequestCloseOrder();
								break;
							}
				case REQ_HOLDPOSITIONTOTAL:
							{
								rettmp = temp[th_handle->hIndex].RequestHoldPositionTotal();
								break;
							}
				case ON_OPENMARKET:
							{

								int mDirector = OPENDIRECTOR_BUY;
								int mQuantity = 1;			
								OpenMarketOrderParam mOrder1;
								mOrder1.nCommodityID = mCommodityID;
								mOrder1.nOpenDirector = mDirector;
								mOrder1.nQuantity = mQuantity;
								mOrder1.dbTradeRange = 50;

								RealTimeQuote mQuote;
								rettmp = temp->QueryCommodityQuotebyID(mCommodityID,mQuote);

								mOrder1.dbPrice = (mDirector == OPENDIRECTOR_BUY ? mQuote.BuyPrice : mQuote.SellPrice);
								rettmp = temp->OnOpenMarketOrder(mOrder1);
								break;

							}
				case ON_CLOSEMARKET:
							{

								long long mID = 0;
								int mQuantity = 0;
								double mPrice = 0;

								RealTimeQuote mQuote;
								rettmp = temp->QueryCommodityQuotebyID(mCommodityID,mQuote);

								printf("KEYIN CloseOrder Param: HoldPostionID Quantity Price\neg. 46272462343 3 258.12 --for close 3g by price 258.12\n");
								scanf("%lld%d%lf",&mID,&mQuantity,&mPrice);

								CloseMarketOrderParam mOrder2;
								mOrder2.nCommodityID = mCommodityID;
								mOrder2.nHoldPositionID = mID;
								mOrder2.nQuantity = mQuantity;
								mOrder2.dbPrice = mPrice;
								mOrder2.nTradeRange = 50;

								rettmp =  temp->OnCloseMarketOrder(mOrder2);
								break;
							}
				case ON_CLOSEMARETMANY:
							{

								int mDirector = OPENDIRECTOR_BUY;
								int mQuantity = 1;

								RealTimeQuote mQuote;
								rettmp = temp->QueryCommodityQuotebyID(mCommodityID,mQuote);

								CloseMarketOrderManyParam mOrder4;
								mOrder4.nCommodityID = mCommodityID;
								mOrder4.nCloseDirector = mDirector;
								mOrder4.dbPrice = (mDirector == OPENDIRECTOR_BUY ? mQuote.BuyPrice : mQuote.SellPrice);
								mOrder4.nQuantity = mQuantity;
								mOrder4.nTradeRange = 50;
								rettmp = temp->OnCloseMarketOrderMany(mOrder4);
								break;

							}
				case ON_OPENLIMIT:
							{

								int mDirector = OPENDIRECTOR_BUY;
								int mQuantity = 1;
								double mPrice = 0;
								double mSLPrice = 0;
								double mTPPrice = 0;

								RealTimeQuote mQuote;
								rettmp = temp->QueryCommodityQuotebyID(mCommodityID,mQuote);

								OpenLimitOrderParam mOrder5;
								mOrder5.nCommodityID = mCommodityID;
								mOrder5.nExpireType = 1;
								mOrder5.nOpenDirector = mDirector;
								mOrder5.dbOrderPrice = mPrice;
								mOrder5.nQuantity = mQuantity;
								mOrder5.dbSLPrice = mSLPrice;
								mOrder5.dbTPPrice = mTPPrice;
								rettmp = temp->OnOpenLimitOrder(mOrder5);
								break;
							}
				case ON_CLOSELIMIT:
							{

								int mQuantity = 1;
								int mID = 0;
								double mClosePrice = 0;
								double mSLPrice = 0;
								double mTPPrice = 0;

								RealTimeQuote mQuote;
								rettmp = temp->QueryCommodityQuotebyID(mCommodityID,mQuote);

								printf("KEYIN CloseLimitOrder Param: HoldOrderID  Quantity  ClosePrice  SLPrice  TPPrice\neg. 46272462343 3 258.12  278.12  238.12 \n--for Close Order[46272462343] 3g on 258.12, set SL 238.12 TP 258.12\n");
								scanf("%d%d%lf%lf%lf", &mID, &mQuantity, &mClosePrice, &mSLPrice, &mTPPrice);

								CloseLimitOrderParam mOrder5;
								mOrder5.nCommodityID = mCommodityID;
								mOrder5.nExpireType = 1;
								mOrder5.nHoldPositionID = mID;
								mOrder5.dbClosePrice = mClosePrice;
								mOrder5.dbSLPrice = mSLPrice;
								mOrder5.dbTPPrice = mTPPrice;
								rettmp = temp->OnLimitClosePosition(mOrder5);
								break;
							}
				case ON_OPENDELIVERYORDER:
							{
								long long mHoldID = 12345678;
								double mHoldWeight = 2;
								double mHoldPrice = 3000;

								OpenDeliveryOderConf mOpenDeliveryOderConf;
								rettmp = temp[th_handle->hIndex].GetOpenDeliveryOrderConf(mCommodityID,mHoldWeight,mHoldPrice,mOpenDeliveryOderConf);
								///TODO show the delivery charge and commission to the client
								OpenDeliveryOrderParam mOrder7;
								mOrder7.nCommodityID = mCommodityID;
								mOrder7.nHoldPositionID = mHoldID;
								mOrder7.nDeliveryCommodityID = 100000;
								rettmp = temp[th_handle->hIndex].OnOpenDeliveryOrder(mOrder7);
								break;
							}
				case ON_CUSTMTRADEREPORTHOLDPOSITOIN:
							{
								time_t sec;
								time(&sec);
								ReportQueryParam mParam;

								mParam.nQueryDateType = 2;//查询类型1：当前   2：历史（目前只支持2模式）
								mParam.nBeginDate = sec - 20*86400;//起始时间，utc时间
								mParam.nEndDate = sec;//终止时间，utc时间
								mParam.nBeginRow = 1; //开始记录序号
								mParam.nEndRow = 30; //结束记录序号

								rettmp = temp[th_handle->hIndex].OnCustmTradeReportHoldPosition(mParam);
								sleep(10);
								break;
							}
				case ON_CUSTMTRADEREPORTCLOSEPOSITOIN:
							{
								time_t sec;
								time(&sec);
								ReportQueryParam mParam;

								mParam.nQueryDateType = 2;                                                     
								mParam.nBeginDate = sec - 20*86400;
								mParam.nEndDate = sec;
								mParam.nBeginRow = 1;
								mParam.nEndRow = 30;

								rettmp = temp[th_handle->hIndex].OnCustmTradeReportClosePosition(mParam);
								sleep(10);
								break;
							}
				case ON_CUSTMTRADEREPORTLIMITORDER:
							{
								time_t sec;
								time(&sec);
								ReportQueryParam mParam;

								mParam.nQueryDateType = 2;                                                     
								mParam.nBeginDate = sec-20*86400;
								mParam.nEndDate = sec;
								mParam.nBeginRow = 1;
								mParam.nEndRow = 30;

								rettmp = temp[th_handle->hIndex].OnCustmTradeReportLimitOrder(mParam);
								break;
							}
				case ON_LIMITREVOKE:
							{

								long long mLimitOrderID = 0;
								int mLimitType = 0;
								rettmp = temp->RequestLimitOrders();

								printf("KEYIN LimitRevoke Param: \neg. 46272462343 1\n--for Revoke the Open&SL&TP of LimitOrder[46272462343] 1:建仓 2:only止盈 3:only止损\n");
								scanf("%lld%d", &mLimitOrderID, &mLimitType);

								LimitRevokeParam mOrder6;
								mOrder6.nCommodityID = mCommodityID;
								mOrder6.nLimitOrderID = mLimitOrderID;
								mOrder6.nLimitType = mLimitType;
								rettmp = temp->OnLimitRevoke(mOrder6);
								break;
							}
				case ON_MONEYQUERY:
							{

								rettmp = temp->OnMoneyQuery();
								break;
							}
				case ON_MONEYINOUT:
							{
								rettmp = temp->OnMoneyQuery();
								usleep(500000);
								MoneyInOutParam mOrder0;
								memset(&mOrder0, 0, sizeof(MoneyInOutParam));
								mOrder0.Amount = 2000.00;
								memcpy(mOrder0.FundPsw, "111111", sizeof(mOrder0.FundPsw));
								memcpy(mOrder0.BankPsw, "111111", sizeof(mOrder0.BankPsw));
								memcpy(mOrder0.Reversed, "*.*.*.*", sizeof(mOrder0.Reversed));
								mOrder0.Currency = 1;
								mOrder0.OperateType = MONEY_IN;
								mOrder0.PayType = 1;//非红包操作请填0
								mOrder0.OperateFlag = 1;//非红包操作请填0
								rettmp = temp->OnMoneyInOut(mOrder0);
								break;
							}
				case ON_LOGINPWDUPD:
							{
								std::string newpwd("000000");
								std::string oldpwd("111111");
								rettmp = temp[th_handle->hIndex].LoginPwdUpd(oldpwd, newpwd, 1);                                        
								sleep(20);
								break;
							}
				case ON_FUNDPWDUPD:
							{
								std::string newpwd("000000");
								std::string oldpwd("111111");
								rettmp = temp[th_handle->hIndex].FundPwdUpd(oldpwd, newpwd);
								
								sleep(20);
								break;
							}
				case ON_FUNDFLOWQUERY:
							{
								time_t sec;
								time(&sec);
								FundFlowQueryParam mParam;

								mParam.nQueryType = 1;                                               
								mParam.nBeginDate = sec - 6*86400;
								mParam.nEndDate = sec - 0*86400;
								mParam.nBeginRow = -1; //开始记录序号
								mParam.nEndRow = -1; //结束记录序号
								mParam.nOperType = 0;
								rettmp = temp[th_handle->hIndex].OnCustomerSelfFundFlowQuery(mParam);
								
								sleep(20);
								break;
							}
				case ON_SIGNRESULTNOTIFYQUERY:
							{
								SignResultNotifyQueryParam mParam;
								mParam.BankID = 0;
								memset(mParam.BankAccount, 0, sizeof(mParam.BankAccount));
								rettmp = temp[th_handle->hIndex].OnSignResultNotifyQuery(mParam);
								break;
							}
				case ON_PAYFORWARD:
							{
								PayForwardParam mParam;
								memcpy(mParam.Ticket, "Ticket", MAX_TICKET_LEN);
								memcpy(mParam.IdentifyCode, "IdentifyCode", MAX_IDENTIFYCODE_LEN);
								memcpy(mParam.Reversed, "*.*.*.*", MAX_IP_LEN);

								rettmp = temp[th_handle->hIndex].OnPayForward(mParam);
								break;
							}
				case ON_CUSTOMERALLOWLOGIN:
							{
								AllowLoginParam mParam;
								mParam.Type = 2;
								mParam.AllowLogin = 0;

								rettmp = temp[th_handle->hIndex].OnCustomerAllowLogin(mParam);
								break;
							}
				case EXIT:
							{
								exit(0);
								break;
							}
				default:
							{
								printf("[TE] unkown event: %d\n", th_handle->hEvent);
								break;
							}
				}
        }
}

void randHandle(handle_event *handleevent,int type, int times, int BASE)
{
	char tmp[][64] = {
		"00300000000xxxx",
		"00300000000xxxx",
	};

	int r_acc = BASE%MAX_USER;
	handleevent->hIndex= r_acc;
	handleevent->hLoginacc= tmp[r_acc];
	handleevent->hEvent= type;
	handleevent->hTimes= times;
}

void* heartbit(void *param)
{
	int m_HeartTimeOut = 20;
	time_t currtime =  time(NULL);
	time_t prevtime = 0;

	while(1)
	{
		map<int, TradeClient*>::iterator it ;
		currtime =  time(NULL);

		if( prevtime + m_HeartTimeOut <= currtime )
		{
			for(it=mClient->begin(); it!=mClient->end();++it)
			{
				it->second->StartHeartBeat();
			}
			prevtime = currtime;
		}
		sleep(1);
	}
	return NULL;
}

int main(int argc, char* argv[])
{
	pthread_t heartbitpid;
	handle_event m_he[MAX_USER];

	AppLog gAppLog;
	gAppLog.InitLog("./", 1, 1);

	QuoteServiceInit gQuoteSer;
	gQuoteSer.InitQuote("127.0.0.1",12345);//根据提供的IP和端口修改
	gQuoteSer.RunQuote("test1","123456");//根据会员自身修改

	int usrnum = 2;
	int eventtype = 0;
	int eventtimes = 0;

	int usrid=0;

	for(; usrid<usrnum; usrid++)
	{
		randHandle(m_he+usrid, eventtype, eventtimes, usrid);
	}

	pthread_create(&heartbitpid, NULL, heartbit, NULL);
	while(1)
	{
		usrid = 0;
		do{

            usleep(10000);

            printf("\n[%s][%d]>>>>>>>>>>[%s][%d][%d],online[%d]\n",getime().c_str(),usrid, m_he[usrid].hLoginacc.c_str(), m_he[usrid].hIndex,m_he[usrid].hEvent, countusr);
			usrprocess(m_he+usrid);
		}while(++usrid < usrnum);
	}

	pthread_join(heartbitpid, NULL);
    delete []temp;
	gQuoteSer.ReleaseQuote();
    gAppLog.ReleaseLog();
    return 0;
} 

