想大家提供一个makefile万能模板，工程目录结构project目录下有三个文件夹，include文件夹放头文件，src放cpp文件，lib文件夹放lib.a文件lib.so文件都可以的以及其他链接文件都可以的。

说明：根据需要大家可以随便扩展这个模板，在任何位置放置自己的头文件，源文件和库文件都可以，只要把路径添加makefile中去就可以了。


```
CC = gcc
XX = g++


CFLAGS = -W -O0 -fPIC

target = ./run_test

#放置头文件路径
includes =  -I ./ \
            -I ../ \
            -I ./include \
#放置cpp的路径加在这儿
user_dir =  ./ \
            ./src

#静态库和动态库路径放这儿
lib_path =  -L ./   \
            -L ../ \
            -L ./lib

#link lib.a file
lib = -lpthread \
	
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
```
