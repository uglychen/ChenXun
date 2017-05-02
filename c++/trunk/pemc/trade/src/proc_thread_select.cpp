#include "proc_thread_select.h"
#include "zlog.h"

proc_thread_select& g_proc_thread_select()
{
	static proc_thread_select data;
	return data;
}

proc_thread_select::proc_thread_select()
	:_out_interval(30 * 24 * 3600) //默认超时一个月的时间
{
	
}
proc_thread_select::~proc_thread_select()
{

}
bool proc_thread_select::init(time_t out_interval)
{
	_out_interval = out_interval;
	return _select_map_mutex.init();
}
pthread_t proc_thread_select::get_select(const std::string& key)
{
	dzlog_info("thread select, key = %s", key.c_str());
	pthread_t ret = 0;
	boost::unordered_map<std::string, proc_thread_node>::iterator iter;
	_select_map_mutex.lock();
	iter = _select_map.find(key);
	if (_select_map.end() != iter)
	{
		//判断是否超时, 查看下是否超時
		/*if (time(NULL) - iter->second._last_select > _out_interval)
		{
			_select_map.erase(iter);
			ret = new_thread();
			if (0 != ret)
			{
				proc_thread_node new_node;
				new_node.key = key;
				new_node._last_select = time(NULL);
				new_node._pthread_id = ret;
				_select_map.insert(std::pair<std::string, proc_thread_node>(key, new_node));
			}
			else
			{

			}
		}
		else
		{*/
			ret = iter->second._pthread_id;
		//}		
	}
	else
	{
		//新连接，判断哪个线程的用户量少就选哪个
		ret = new_thread();

		if (0 != ret)
		{
			proc_thread_node new_node;
			new_node.key = key;
			new_node._last_select = time(NULL);
			new_node._pthread_id = ret;
			_select_map.insert(std::pair<std::string, proc_thread_node>(key, new_node));
			dzlog_info("select new thread, key = %s, id = %ld", key.c_str(), ret);
		}
		else
		{
			dzlog_info("select thread error, key = %s", key.c_str());
		}
	}
	_select_map_mutex.unlock();
	dzlog_info("select thread, key = %s, id = %ld", key.c_str(), ret);
	return ret;
}
pthread_t proc_thread_select::new_thread()
{
	dzlog_info("new thread, map size = %lu", _thread_count_map.size());
	pthread_t ret = 0;
	//新连接，判断哪个线程的用户量少就选哪个
	int min_count = std::numeric_limits<int>::max();
	boost::unordered_map<pthread_t, int>::iterator count_iter = _thread_count_map.begin();
	boost::unordered_map<pthread_t, int>::iterator count_end = _thread_count_map.end();
	for (; count_iter != count_end; ++count_iter)
	{
		if (min_count > count_iter->second)
		{
			ret = count_iter->first;
			min_count = count_iter->second;
		}
	}

	return ret;
}
pthread_t proc_thread_select::del_select(const std::string& key)
{
	pthread_t ret = 0;
	boost::unordered_map<std::string, proc_thread_node>::iterator iter;
	_select_map_mutex.lock();
	iter = _select_map.find(key);
	if (_select_map.end() != iter)
	{
		ret = iter->second._pthread_id;
		_select_map.erase(iter);

		boost::unordered_map<pthread_t, int>::iterator count_iter = _thread_count_map.find(ret);
		if (_thread_count_map.end() != count_iter && count_iter->second > 0)
		{
			--count_iter->second;
		}
	}
	_select_map_mutex.unlock();

	return ret;
}
void proc_thread_select::update_thread_user_count(pthread_t thread_id, unsigned long count)
{
	boost::unordered_map<pthread_t, int>::iterator iter;
	_select_map_mutex.lock();
	iter = _thread_count_map.find(thread_id);
	if (_thread_count_map.end() != iter)
	{
		iter->second = count;
	}
	else
	{
		_thread_count_map.insert(std::pair<pthread_t, int>(thread_id, count));
	}
	_select_map_mutex.unlock();
}

void proc_thread_select::destory()
{

}