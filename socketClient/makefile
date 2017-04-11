CC = gcc
XX = g++ -std=c++11


CFLAGS = -W -O0 -fPIC -fpermissive

target = ./run_test

PAHO_C_LIB=/root/paho_mqtt/paho.mqtt.c
PAHO_CPP_LIB=/root/paho_mqtt/paho.mqtt.cpp

#放置头文件路径
includes =  -I ./ \
            -I ../ \
            -I ./include \
            -I ./include/mqtt\
            -I /usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.4\
            -I /usr/local/apr/include/apr-1/\


#放置cpp的路径加在这儿
user_dir =  ./ \
            ./src

#静态库和动态库路径放这儿
lib_path =  -L ./   \
            -L ../ \
            -L ./lib\
            -L /usr/local/ActiveMQ-CPP/lib/\
            

#link lib.a file
lib =   -lpthread \
        -lmqttpp\
        -lpaho-mqtt3a\
        -lactivemq-cpp\


INCLUDE := $(includes)


files = $(foreach d, $(user_dir), $(wildcard $(d)/*.cpp))
objects = $(patsubst %.cpp, %.o, $(files))

$(target) : $(objects)
	$(XX)  $^ -o $@  $(lib_path) $(lib)

%.o : %.cpp
	$(XX) $(CFLAGS) $(includes) -c  $^  -g -o  $@  -DLinux

clean :
	rm -rf $(target)
	rm -rf $(objects)
