#include "proc_task.h"

proc_task::proc_task(int type)
	:_proc_type(type)
{

}
proc_task::~proc_task()
{

}


/***************************************************************************************************************************/
request_task::request_task()
	:proc_task(proc_task_type_request)
{

}

request_task::~request_task()
{

}

void request_task::clear()
{
	_account.clear();
	_type.clear();
}

std::string request_task::key() const
{
	return _account;
}














/***************************************************************************************************************************/
callback_task::callback_task()
	:proc_task(proc_task_type_callback)
	,_event_type(-1)
	, _success(false)
{

}

callback_task::~callback_task()
{

}

std::string callback_task::key() const
{
	return _account;
}

