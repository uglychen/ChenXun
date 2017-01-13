# strchar函数的实现
* char *strchr(const char* _Str,char _Val)
* char *strchr(char* _Str,char _Ch)
* 头文件：#include <string.h>
* 功能：查找字符串_Str中首次出现字符_Val的位置
* 说明：返回首次出现_Val的位置的指针，返回的地址是被查找字符串指针开始的第一个与Val相同字符的指针，如果Str中不存在Val则返回NULL。
* 返回值：成功则返回要查找字符第一次出现的位置，失败返回NULL
```
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

char *my_strchar(char *str, char c)
{
    while( *str!='\0' && *str!=c)
    {
        str++;
    }
    return *str==c?str:NULL;
}

int main()
{
    char *str = "chenxun";
    char *value = my_strchar(str,'x');
    std::cout<< value << std::endl;
    return 0;
}

```

# strstr函数
* 包含文件：string.h
* 函数名: strstr
* 函数原型：extern char *strstr(char *str1, const char *str2);
* 语法：strstr(str1,str2)
str1: 被查找目标　string expression to search.
str2: 要查找对象　The string expression to find.
* 返回值：若str2是str1的子串，则返回str2在str1的首次出现的地址；如果str2不是str1的子串，则返回NULL。


```
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

char *my_strstr(const char *str1, const char *str2)
{
    char *cp = (char*)str1;
    char *s1,*s2; 
    if(!*str2)
        return cp; 

    while(*cp)
    {
        s1 = cp;
        s2 = (char*)str2;
        while(*s1 && *s2 && !(*s1-*s2))
        {
            s1++, s2++;
        }
        if(!*s2)
            return cp;     
        cp++;
    }    
    return NULL;
}

int main()
{
    char *str = "chenxun";
    char *value = my_strstr(str,"xun");
    std::cout << value << std::endl;
    return 0;
}
```
[link]http://baike.baidu.com/link?url=Yzm50c9dYr6tDjfiJv0DrZ7YSiT8O4u1a9eaFDF1zMfE9BfVVpwcWjRvsCxeE02ONdUvZ5T7o9NP8cWg1C0_0K