#include "config.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <exception>
#include <stdio.h>
#include <string.h>
#include "zlog.h"

config& g_config()
{
	static config data;
	return data;
}
config::config()
{

}
config::~config()
{

}
bool config::init(const char* ptr_path)
{
	// printf("%s,%d,notice: load config start \n", __FILE__, __LINE__);
	dzlog_info("notice: load config start");
	static const int buffer_size = 10240;
	char buffer[buffer_size];
	std::ifstream config_file(ptr_path, std::ios::in);
	if (!config_file.is_open())
	{
		// printf("%s,%d,error: can`t open config file \n", __FILE__, __LINE__);
		dzlog_error("error: can`t open config file");
		return false;
	}

	config_file.read(buffer, buffer_size - 1);
	int count = config_file.gcount();
	if (count == 0)
	{
		// printf("%s,%d,error: can`t read info from file \n", __FILE__, __LINE__);
		dzlog_error("error: can`t read info from file");
		return false;
	}

	buffer[count] = '\0';
	
	if (strlen(buffer) == 0)
	{
		// printf("%s,%d,error: json is empty \n", __FILE__, __LINE__);
		dzlog_error("error: json is empty");
		return false;
	}

	try
	{
		//½âÎöjson
		Json::Reader reader(Json::Features::strictMode());
		if (false == reader.parse(buffer, _kv))
		{
			// printf("%s,%d,error: json parser error \n", __FILE__, __LINE__);
			dzlog_error("error: json parser error");
			return false;
		}
	}
	catch (...)
	{
		// printf("%s,%d,error: json  exception\n", __FILE__, __LINE__);
		dzlog_error("error: json  exception");
		return false;
	}

	// printf("%s,%d,notice: load config success \n", __FILE__, __LINE__);
	dzlog_notice("notice: load config success");
	return true;

}
bool config::get_value(const char* key, std::string& value)
{
	try
	{
		Json::Value js_value = _kv[key];
		if (js_value.isNull() || !js_value.isString())
		{
			return false;
		}
		else
		{
			value = js_value.asString();
			return true;
		}
	}
	catch (...)
	{
		// printf("%s,%d,error: json exception \n", __FILE__, __LINE__);
		dzlog_error("error: json exception");
		return false;
	}
}
bool config::get_value(const char* key, double &value)
{
	try
	{
		Json::Value js_value = _kv[key];
		if (js_value.isNull() || !js_value.isDouble())
		{
			return false;
		}
		else
		{
			value = js_value.asDouble();
			return true;
		}
	}
	catch (...)
	{
		// printf("%s,%d,error: json exception \n", __FILE__, __LINE__);
		dzlog_error("error: json exception");
		return false;
	}
}
bool config::get_value(const char* key, bool &value)
{
	try
	{
		Json::Value js_value = _kv[key];
		if (js_value.isNull() || !js_value.isBool())
		{
			return false;
		}
		else
		{
			value = js_value.asBool();
			return true;
		}
	}
	catch (...)
	{
		// printf("%s,%d,error: json exception \n", __FILE__, __LINE__);
		dzlog_error("error: json exception");
		return false;
	}
}
bool config::get_value(const char* key, int &value)
{
	try
	{
		Json::Value js_value = _kv[key];
		if (js_value.isNull() || !js_value.isInt())
		{
			return false;
		}
		else
		{
			value = js_value.asInt();
			return true;
		}
	}
	catch (...)
	{
		// printf("%s,%d,error: json exception \n", __FILE__, __LINE__);
		dzlog_error("error: json exception");
		return false;
	}
}
bool config::get_value(const char* key, unsigned int &value)
{
	try
	{
		Json::Value js_value = _kv[key];
		if (js_value.isNull() || !js_value.isUInt())
		{
			return false;
		}
		else
		{
			value = js_value.asUInt();
			return true;
		}
	}
	catch (...)
	{
		// printf("%s,%d,error: json exception \n", __FILE__, __LINE__);
		dzlog_error("error: json exception");
		return false;
	}
}
//bool config::get_value(const char* key, long long &value)
//{
//	Json::Value js_value = _kv[key];
//	if (js_value.isNull() || !js_value.isInt64())
//	{
//		return false;
//	}
//	else
//	{
//		value = js_value.asInt64();
//		return true;
//	}
//}
//bool config::get_value(const char* key, unsigned long long &value)
//{
//	Json::Value js_value = _kv[key];
//	if (js_value.isNull() || !js_value.isUInt64())
//	{
//		return false;
//	}
//	else
//	{
//		value = js_value.asUInt64();
//		return true;
//	}
//}