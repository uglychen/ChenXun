
XX = g++

CFLAGS = -W -O0 -fPIC

target = ./run_test

#放置头文件路径
includes =  -I ./ \
            -I/root/software/mysql-connector-c++-1.1.8-linux-glibc2.5-x86-64bit/include\
			-I/root/software/mysql-connector-c++-1.1.8-linux-glibc2.5-x86-64bit/include/cppconn\


#放置cpp的路径加在这儿
user_dir =  ./ \
            ./src

#静态库和动态库路径放这儿
lib_path =  -L ./   \
			-L/root/software/mysql-connector-c++-1.1.8-linux-glibc2.5-x86-64bit/lib\
#link lib.a file
lib = -lpthread \
	-lmysqlcppconn\
 	
files = $(foreach d, $(user_dir), $(wildcard $(d)/*.cpp))
objects = $(patsubst %.cpp, %.o, $(files))

$(target) : $(objects)
	$(XX)  $^ -o $@  $(lib_path) $(lib)

%.o : %.cpp
	$(XX) $(CFLAGS) $(includes) -c  $^  -g -o  $@  -DLinux

clean : 
	rm -rf $(target)
	rm -rf $(objects)
