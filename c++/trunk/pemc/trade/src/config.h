#ifndef _CONFIG_H_52AEE469_7394_4779_B74C_BB22890C6446_
#define _CONFIG_H_52AEE469_7394_4779_B74C_BB22890C6446_
#include <jsoncpp/json.h>
#include <string>

class config
{
private:
	config();
public:
	~config();
	bool init(const char* ptr_path);

	bool get_value(const char* key, std::string& value);
	bool get_value(const char* key, double &value);
	bool get_value(const char* key, bool &value);
	bool get_value(const char* key, int &value);
	bool get_value(const char* key, unsigned int &value);
	//bool get_value(const char* key, long long &value);
	//bool get_value(const char* key, unsigned long long &value);
private:
	Json::Value _kv;

	friend config& g_config();
};


config& g_config();
#endif // !_CONFIG_H_52AEE469_7394_4779_B74C_BB22890C6446_
