#include "recv_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "struct.h"
#include "proc_thread_select.h"
#include "proc_thread.h"
#include <jsoncpp/json.h>
#include "zlog.h"
#include "proc_task.h"

recv_thread& g_recv_thread()
{
	static recv_thread data;
	return data;
}

void recv_thread::on_recv(const std::string& request, std::string& response, const Message* message)
{
	dzlog_info("recv request = %s ", request.c_str());
	response.clear();

	try
	{
		//json decode
		Json::Value value;
		Json::Reader reader(Json::Features::strictMode());
		if (true == reader.parse(request.c_str(), value))
		{
			request_task* task = new request_task;
			boost::shared_ptr<proc_task> ptr_task = boost::shared_ptr<proc_task>(task);
			Json::Value js_type = value["Type"];
			if (js_type.isNull() || !js_type.isString())
			{
				dzlog_info("type is error \n");
				return;
			}
			else
			{
				task->_type = js_type.asString();
			}

		
			static const std::string access_type = "pemc.mid.trade.";
			static const unsigned access_type_len = access_type.size();
			if (task->_type.size() < access_type_len)
			{
				dzlog_info("type length is so short, len = %lu ", task->_type.size());
				return;
			}

			if (strncmp(access_type.c_str(), task->_type.c_str(), access_type.size()) != 0)
			{
				dzlog_info("type is not access, type = %s ", task->_type.c_str());
				return;
			}

			task->_type = task->_type.c_str() + access_type_len;

			task->_param = value["Param"];
			if (task->_param.isNull() )
			{
				dzlog_info("param is error \n");
				return;
			}
			else
			{
				Json::Value js_account = task->_param["Account"];
				if (js_account.isNull() || !js_account.isString())
				{
					dzlog_info("acount is error \n");
					return;
				}
				else
				{
					task->_account = js_account.asString();
				}
			}

			if (task->_account.empty())
			{
				dzlog_info("acount is empty \n");
				return;
			}

			task->_message = boost::shared_ptr<TextMessage>(dynamic_cast<TextMessage*>(message->clone()));
			if (NULL == task->_message.get())
			{
				dzlog_info("destination is empty \n");
				return;
			}
			
			proc_thread_select& select = g_proc_thread_select();
			proc_thread_container& thread_container = g_proc_thread_container();

			//判断用户使用哪个处理线程，分新用户和老用户
			pthread_t thread_id = select.get_select(task->key());
			if (0 != thread_id)
			{
				proc_thread* ptr_thread = thread_container.get_thread(thread_id);
				if (NULL != ptr_thread)
				{
					dzlog_info("thread is selected, id = %lu", thread_id);
					unsigned long message_count;
					//将消息发送到对应的处理线程， 处理数据
					ptr_thread->_request_buffer.push(ptr_task, message_count);
					//更新消息队列数据
					select.update_thread_user_count(thread_id, message_count);
				}
				else
				{
					dzlog_info("thread is not selected, thread point is NULL\n");
				}
			}
			else
			{
				dzlog_info("thread is not selected, thread id is zero\n");
			}
			
		}
		else
		{
			dzlog_info("json parse is error\n");
			return;
		}
	}
	catch (...)
	{
		dzlog_info("json parse is error\n");
	}
}

recv_thread::recv_thread()
	:_consumer(this)
{

}

recv_thread::~recv_thread()
{

}

bool recv_thread::init()
{
	std::string uri = "failover:(tcp://172.18.15.7:61616,tcp://172.18.15.10:61616)?randomize=false";
	std::string queue = "cgi_pemc_mid_trade_req";
	if (false == _consumer.init(uri, queue))
	{
		dzlog_error("Initial consumer error\n");
		return false;
	}
	return true;

}

void recv_thread::destory()
{
	_consumer.close();
}