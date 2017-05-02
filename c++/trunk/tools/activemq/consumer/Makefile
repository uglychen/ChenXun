XX = g++ -std=c++11

CFLAGS = -W -O0 -fPIC

target = ./consumer

#放置头文件路径
includes =  -I ./ \
			-I/usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.3/\
			-I/usr/local/apr/include/apr-1\


#放置cpp的路径加在这儿
user_dir =  ./ \
            ./src\

#静态库和动态库路径放这儿
lib_path =  -L ./   \
			-L/usr/local/ActiveMQ-CPP/lib\


#link lib.a file
lib = -lpthread \
	-lactivemq-cpp\
 	
files = $(foreach d, $(user_dir), $(wildcard $(d)*.cpp))
objects = $(patsubst %.cpp, %.o, $(files))

$(target) : $(objects)
	$(XX)  $^ -o $@  $(lib_path) $(lib)

%.o : %.cpp
	$(XX) $(CFLAGS) $(includes) -c  $^  -g -o  $@  -DLinux

clean : 
	rm -rf $(target)
	rm -rf $(objects)
