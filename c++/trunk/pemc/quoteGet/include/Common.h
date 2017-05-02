#ifndef _COMMON_H_
#define _COMMON_H_

extern const std::string log_config_file_path;
extern int server_prot;
extern const std::string  server_ip;
extern const std::string brokerURI;
extern bool useTopic;
extern bool clientAck;
extern std::string topic;
extern const std::string db_url;
extern const std::string user;
extern const std::string passwd;
extern int connect_num;

void read_config();
void read_config_xml();
void read_config_tinyxml();
void init_quote();

#endif