### snprintf()  格式化输出字符串函数
>#include<stdio.h>  
`int snprintf(char *str, size_t size, const char *format, ...);`  
将可变参数`...`按照`format`格式化成字符串，并将字符串复制到str中，size为要写入的字符的最大数目，超过size会被截断。  
```c
char buffer[50];
char *name = "test1234";
int j = snprintf(buffer, 6, "%s");

//输出为test1，因为后面包含了一个\0
>>test1
```


### strdub() 字符串拷贝函数  
>#include<string.h>  
`char * strdub(char *s);`实现字符串拷贝，内部调用了`malloc`为变量分配内存，因此返回的字符串指针需要调用`free()`释放。如果分配失败则返回NULL。  
```c
char *s = "test1234";
char *p;
p = strdup(s);
free(p);
```  


### strchr() 字符串匹配函数  
>#include<string.h>  
`char * strchr(const char *str, int c);`在参数str所指向的字符串中搜索第一次出现字符c(一个无符号字符)的位置。 
```c
char str[20];
strcyp(str,"test_strchr");
char *p = null;
char c = '_';
p = strchr(str,c);
printf("%s");
>>_strchr
```
