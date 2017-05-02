#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "gd_trade.h"
#include <map>
#include "proc_thread.h"
#include "recv_thread.h"
#include "proc_thread_select.h"
#include "enum.h"
#include "config.h"
#include <string.h>
#include <string>
#include "zlog.h"
#include "tinyxml.h"
#include "readXMLConfig.h"



volatile bool run = true;
void handler_sig_stop(int signo, siginfo_t *siginfo, void *pvoid)
{
	dzlog_info("handler info, sig no = %d", signo);
	run = false;
}

void add_signal_stop()
{
	/*struct sigaction act;
	act.sa_sigaction = handler_sig_stop;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &act, NULL);*/
}

int main(int argc, char* argv[])
{	
	// zlog init
	int /*rc,*/ dzlogrt;
	zlog_category_t *c;

	//rc = zlog_init("zlog.conf");
	dzlogrt = dzlog_init("zlog.conf", "trade");

	// if (rc) 
	// {
	// 	printf("zlog init failed\n");
	// 	return -1;
	// }

	if (dzlogrt) 
	{
		printf("file:%s, line:%d, dzlogrt init failed\n", __FILE__, __LINE__);
		return -1;
	}

	c = zlog_get_category("my_cat");

	if (!c) 
	{
		//printf("file:%s, line:%d, get cat fail\n" __FILE__, __LINE__);
		zlog_fini();
		return -2;
	}

	dzlog_info("----------------------zlog init end----------------------");
	// printf("----------------------zlog init end----------------------\n");

	if (argc >= 2)
	{
		// printf("argv[1]=%s\n", argv[1]);
		dzlog_info("argv[1]=%s", argv[1]);
		if (strcmp(argv[1], "start") == 0)
		{
			// printf("----------------------Strat pemc middle trade serveice----------------------\n");
			dzlog_info("----------------------Strat pemc middle trade serveice----------------------");
			config& config = g_config();

			static readXMLConfig * readConfig = new readXMLConfig();
			static string strpath = "./config.xml";  
			
			if (false == config.init("./config.json"))
			{
				// printf("load config file error\n");
				dzlog_error("load config file error");
				return -1;
			}

			AppLog gAppLog;		//日志
			QuoteServiceInit gQuoteSer; //服务环境

			while (1)
			{
				// printf("----------------------init activemq----------------------\n");
				dzlog_info("----------------------init activemq----------------------");
				//初始化MQ环境
				activemq::library::ActiveMQCPP::initializeLibrary();
				dzlog_info("----------------------init sdk log----------------------");
				//初始化环境
				std::string app_log_path;
				std::string tmp_app_log_path_ele_name = "app_log_path";
				// if (false == config.get_value("app_log_path", app_log_path))
				// {
				// 	app_log_path = "./applog";
				// }

				if (0 != readConfig->getNodeValueByName(strpath, tmp_app_log_path_ele_name, app_log_path))
				{
					app_log_path = "./applog";
				}
				// printf("log path = %s\n", app_log_path.c_str());
				dzlog_info("log path = %s", app_log_path.c_str());
				if (0 != gAppLog.InitLog(app_log_path.c_str(), 1, 1))
				{
					// printf("init log path error\n");
					dzlog_error("init log path error");
					break;
				}

				dzlog_info("----------------------init quote----------------------");
				//SDK初始化
				//初始化报价
				std::string server_ip;
				std::string tmp_server_ip_ele_name = "server_ip";
				int int_server_port;
				std::string server_port;
				std::string tmp_server_port_ele_name = "server_port";
				// if (false == config.get_value("server_ip", server_ip))
				// {
				// 	server_ip = "127.0.0.1";
				// }
				// if (false == config.get_value("server_port", server_port))
				// {
				// 	server_port = 1234;
				// }
				if (0 != readConfig->getNodeValueByName(strpath, tmp_server_ip_ele_name, server_ip))
				{
					server_ip = "127.0.0.1";
				}
				if (0 != readConfig->getNodeValueByName(strpath, tmp_server_port_ele_name, server_port))
				{
					int_server_port = 1234;
				}
				else
				{
					int_server_port = atoi(server_port.c_str());
				}
				
				// printf("server ip = %s, port = %d\n", server_ip.c_str(), server_port);
				dzlog_info("server ip = %s, port = %d", server_ip.c_str(), int_server_port);

				if (0 != gQuoteSer.InitQuote(server_ip, int_server_port))//根据提供的IP和端口修改
				{
					// printf("init quote server evn error\n");
					dzlog_error("init quote server evn error");
					break;
				}

				dzlog_info("----------------------run quote----------------------");
				//会员账号密码-公司对广贵		
				std::string account, passwd;
				std::string tmp_account_ele_name = "account";
				std::string tmp_passwd_ele_name = "passwd";	
				// if (false == config.get_value("account", account))
				// {
				// 	account = "test1";
				// }
				// if (false == config.get_value("passwd", passwd))
				// {
				// 	passwd = "123456";
				// }
				if (0 != readConfig->getNodeValueByName(strpath, tmp_account_ele_name, account))
				{
					account = "test1";
				}
				if (0 != readConfig->getNodeValueByName(strpath, tmp_passwd_ele_name, passwd))
				{
					passwd = "123456";
				}
				dzlog_info("account = %s, passwd = %s", account.c_str(), passwd.c_str());
				if (0 != gQuoteSer.RunQuote(account, passwd))//根据会员自身修改
				{
					// printf("quote server run error\n");
					dzlog_error("quote server run error");
					break;
				}


				if (false == g_request_type2int().init())
				{
					// printf("request type init error\n");
					dzlog_error("request type init error");
					break;
				}

				dzlog_info("----------------------init thread select----------------------");
				//初始化线程选择
				int int_out_of_time_interval;
				string out_of_time_interval;
				string tmp_out_of_time_interval_ele_name = "out_of_time_interval";
				// if (false == config.get_value("out_of_time_interval", out_of_time_interval))
				// {
				// 	out_of_time_interval = 3600;
				// }
				if (0 != readConfig->getNodeValueByName(strpath, tmp_out_of_time_interval_ele_name, out_of_time_interval))
				{
					int_out_of_time_interval = 3600;
				}
				else
				{
					int_out_of_time_interval = atoi(out_of_time_interval.c_str());
				}
				
				proc_thread_select &thread_select = g_proc_thread_select();
				if (false == thread_select.init(int_out_of_time_interval))
				{
					// printf("thread select init error\n");
					dzlog_error("thread select init error");
					break;
				}

				dzlog_info("----------------------init process thread----------------------");
				//初始化处理线程,启动处理线程
				int proc_thread_num = 10;
				for (int i = 0; i < proc_thread_num; ++i)
				{
					if (false == g_proc_thread_container().add_thread())
					{
						// printf("add thread error\n");
						dzlog_info("add thread error");
					}
				}

				dzlog_info("----------------------init recv thread----------------------");
				//初始化接收线程,启动接收线程
				if (false == g_recv_thread().init())
				{
					// printf("recv thread init error\n");
					dzlog_error("recv thread init error");
					break;
				}

				dzlog_info("----------------------wait----------------------");
				//等待信号退出
				add_signal_stop();
				while (run)
				{
					sleep(1);
				}

				break;
			}
			dzlog_info("----------------------stop----------------------");
			//销毁
			g_recv_thread().destory();
			g_proc_thread_container().destory();
			g_proc_thread_select().destory();
			gQuoteSer.ReleaseQuote();
			gAppLog.ReleaseLog();
			activemq::library::ActiveMQCPP::shutdownLibrary();
			dzlog_info("---------------------- Pemc middle trade serveice stop ----------------------");
		}
		else if(strcmp(argv[1], "stop") == 0)
		{
			dzlog_info("---------------------- Stop pemc middle trade serveice ----------------------");
			/*union sigval value;
			value.sival_int = atoi(argv[i + 1]);		
			int signum = atoi(argv[i + 1]);
			pid_t pid = atoi(argv[i + 1]);
			if (sigqueue(pid, signum, value)<0)
			{
				printf("Stop error\n");
			}
			else
			{
				printf("Stopping\n");
			}
			printf("Stop success\n");*/
		}
		else
		{
			dzlog_info("---------------------- error ----------------------");
		}
	}
	else
	{
		dzlog_info("----------------------help----------------------");
		dzlog_info("start  : start service");
		dzlog_info("stop   : stop service");
		dzlog_info("------------------------------------------------");
	}

	zlog_fini();
	return 0;
}

