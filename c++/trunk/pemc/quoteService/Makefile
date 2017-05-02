XX = g++ -std=c++11

CFLAGS = -W -O0 -fPIC

target = ./quoteService

#放置头文件路径
includes =  -I ./ \
			-I/usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.4/\
			-I/usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.3/\
			-I/usr/local/apr/include/apr-1\
			-I/usr/include/mysql-connect/\
			-I ../../tools/activemq/consumer/\
			-I ../../include/\
			

#放置cpp的路径加在这儿
user_dir =  ./ \
            ./src\
			../../tools/activemq/consumer/\
			

#静态库和动态库路径放这儿
lib_path =  -L ./   \
			-L/usr/local/ActiveMQ-CPP/lib\
			-L/usr/lib/mysql-connect\
			-L/usr/local/lib\
			

#link lib.a file
lib = -lpthread \
	-lactivemq-cpp\
	-luuid\
	-lmysqlcppconn\
	-ljson_linux-gcc-4.4.7_libmt\
	-lmqttpp\
    -lpaho-mqtt3a\
	-lzlog\
	
files = $(foreach d, $(user_dir), $(wildcard $(d)*.cpp))
objects = $(patsubst %.cpp, %.o, $(files))

$(target) : $(objects)
	$(XX)  $^ -o $@  $(lib_path) $(lib)

%.o : %.cpp
	$(XX) $(CFLAGS) $(includes) -c  $^  -g -o  $@  -DLinux

clean : 
	rm -rf $(target)
	rm -rf $(objects)
