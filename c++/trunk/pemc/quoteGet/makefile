CC = gcc
XX = g++ -std=c++11


CFLAGS = -W -O0 -fPIC -fpermissive -fpermissive -lrt

target = ./run_quote_get

#PAHO_C_LIB=/root/paho_mqtt/paho.mqtt.c
#PAHO_CPP_LIB=/root/paho_mqtt/paho.mqtt.cpp

BASE_LIB=/root/software/chenxun/svn_project/trunk


#放置头文件路径
includes =  -I ./ \
            -I ../ \
            -I ./include \
	    -I /usr/local/include\
	    -I $(BASE_LIB)/include\
            -I /usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.4\
            -I /usr/local/apr/include/apr-1/\
            -I /root/software/zlog/zlog-1.2.7/src\
            -I ../../include/\
	    -I -I/usr/include/mysql-connect/\

#放置cpp的路径加在这儿
user_dir =  ./ \
            ./src\
            ./lib_json\


#静态库和动态库路径放这儿
lib_path =  -L ./   \
            -L ../ \
            -L ./lib\
            -L -L/usr/local/lib\
            -L /usr/local/ActiveMQ-CPP/lib/\
            -L $(BASE_LIB)/lib/mqtt\
            -L /root/software/zlog/zlog-1.2.7/src\
            -L ../../lib\
	    -L -I/usr/include/mysql-connect/\

#link lib.a file
lib =   -lpthread \
        -lmqttpp\
        -lpaho-mqtt3a\
        -lactivemq-cpp\
        -lzlog\
	-luuid\
	-ljson_linux-gcc-4.4.7_libmt\
        -lmysqlcppconn\


INCLUDE := $(includes)


files = $(foreach d, $(user_dir), $(wildcard $(d)/*.cpp))
objects = $(patsubst %.cpp, %.o, $(files))

$(target) : $(objects)
	$(XX)  $(CFLAGS) $^ -o $@  $(lib_path) $(lib)

%.o : %.cpp
	$(XX) $(CFLAGS) $(includes) -c  $^  -g -o  $@  -DLinux

clean :
	rm -rf $(target)
	rm -rf $(objects)
