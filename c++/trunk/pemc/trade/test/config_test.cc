#include "../src/config.h"
#include "gtest/gtest.h"
#include <string.h>

int main(int argc, char** argv)
{
	printf("-------------------- start ------------------------\n");
	::testing::InitGoogleTest(&argc, argv);

	config& conf1 = g_config();
	config& conf2 = g_config();

	EXPECT_EQ(false, conf1.init("./config"));
	EXPECT_EQ(true, conf2.init("../src/config.json"));
	
	std::string app_log_path, server_ip, accout, passwd;
	int server_port, out_of_time_interval;

	EXPECT_EQ(true, conf2.get_value("app_log_path", app_log_path));
	EXPECT_EQ(true, conf2.get_value("server_ip", server_ip));
	EXPECT_EQ(true, conf2.get_value("accout", accout));
	EXPECT_EQ(true, conf2.get_value("passwd", passwd));

	EXPECT_EQ(true, conf2.get_value("server_port", server_port));
	EXPECT_EQ(true, conf2.get_value("out_of_time_interval", out_of_time_interval));

	EXPECT_EQ(0, strcmp(".", app_log_path.c_str()));
	EXPECT_EQ(0, strcmp("127.0.0.1", server_ip.c_str()));
	EXPECT_EQ(0, strcmp("test1", accout.c_str()));
	EXPECT_EQ(0, strcmp("12345", passwd.c_str()));

	EXPECT_EQ(12345, server_port);
	EXPECT_EQ(3600, out_of_time_interval);

	printf("-------------------- end ------------------------\n");

	return 0;
}