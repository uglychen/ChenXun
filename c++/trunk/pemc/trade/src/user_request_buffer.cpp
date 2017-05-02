#include "user_request_buffer.h"

user_request_buffer::user_request_buffer()
{
}
user_request_buffer::~user_request_buffer()
{
}
bool user_request_buffer::init()
{
	return _user_request_mutex.init();
}
void user_request_buffer::push(boost::shared_ptr<proc_task>& request, unsigned long &buffer_count)
{
	_user_request_mutex.lock();
	_user_request_deque.push_back(request);
	buffer_count = _user_request_deque.size();
	_user_request_mutex.unlock();
}

void user_request_buffer::push_front(boost::shared_ptr<proc_task>& request)
{
	_user_request_mutex.lock();
	_user_request_deque.push_front(request);
	_user_request_mutex.unlock();
}
bool user_request_buffer::pop(boost::shared_ptr<proc_task>& request)
{
	bool ret = false;
	if (true == _user_request_mutex.try_lock())
	{
		if (!_user_request_deque.empty())
		{
			request = _user_request_deque.front();
			_user_request_deque.pop_front();
			
			ret = true;
		}
		else
		{

		}
		
		_user_request_mutex.unlock();
	}
	else
	{
		
	}

	return ret;
}