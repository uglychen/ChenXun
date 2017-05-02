#ifndef _USER_REQUEST_BUFFER_H_6ECABE4F_2DF8_4B6E_9A4B_42999418FBE9_
#define _USER_REQUEST_BUFFER_H_6ECABE4F_2DF8_4B6E_9A4B_42999418FBE9_
#include "struct.h"
#include "xn_mutex.h"
#include "proc_task.h"
#include <deque>
#include <boost/shared_ptr.hpp>

/***************************************************************************************************************************/
//用户请求缓冲区
/***************************************************************************************************************************/
class user_request_buffer
{
public:
	user_request_buffer();
	~user_request_buffer();
	/***************************************************************************************************************************/
	//初始化
	/***************************************************************************************************************************/
	bool init();
	/***************************************************************************************************************************/
	//放数据
	/***************************************************************************************************************************/
	void push(boost::shared_ptr<proc_task>& request, unsigned long &buffer_count);
	void push_front(boost::shared_ptr<proc_task>& callback);
	/***************************************************************************************************************************/
	//取数据
	/***************************************************************************************************************************/
	bool pop(boost::shared_ptr<proc_task>& request);

private:
	/***************************************************************************************************************************/
	//用户请求缓冲区
	/***************************************************************************************************************************/
	std::deque< boost::shared_ptr<proc_task> > _user_request_deque;
	/***************************************************************************************************************************/
	//用户请求缓冲区
	/***************************************************************************************************************************/
	xn_mutex _user_request_mutex;
};

#endif // !_USER_REQUEST_BUFFER_H_6ECABE4F_2DF8_4B6E_9A4B_42999418FBE9_
