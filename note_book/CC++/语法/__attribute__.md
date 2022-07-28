# __attribute__机制    
可以设置函数属性、变量属性、类型属性  
其位置约束为：放于声明的尾部";"之前     
attribute是在基本程序代码中加入的辅助信息，编译器可以利用这些辅助信息来帮助自己的CG（code generation），譬如用来优化或者生产特定代码   

### __attribute__ unused 
用于标记一个变量可能未使用，使编译器忽略该警告。  
```C
//用于函数  
void main(int argc __attribute__((unused)), char **argv);

int mypid __attribute__((unused)) = getpid();
```  

### format  
在C语言编程过程中，常常会实现一些可变参数的函数调用（类scanf、printf函数），变参函数默认情况下编译器检查不出参数的类型和个数是否正确。   
`__attribute__((format(printf, 2, 3)))`的作用就是用来解决这个问题，它用于函数声明，作用使提示编译器检查函数调用过程中，可变参数部分按照printf的规则进行检查，若参数的个数或者类型不匹配，编译过程中将会发出警告，注意编译时加上**-Wall**才可以。  
```C
__attribute__((format(archetype,string_index, frist_to_check)))
//archetype：为按照那种风格进行校验，如printf、scanf等 
//string_index: 格式化format字符串所在的位置，如void test(testA, format, ...)，此时为2  
//frist_to_check： 第一个可变参数的位置，如void test(testA, format, ...),此时为3

//实例  
#define CHECK_FMT(a,b)  __attribute__((format(printf, a, b))) 

void eprintf(const char *fmt, ...) CHECK_FMT(1,2)
void eprintf_test()
{
    eprintf("s=%s\n", 4);//error type
    eprintf("%d, %d, %d\n", 1, 2);//error too few arguments
}
``` 


### 多个属性可以串在一起声明  
```C
extern void die(const char *format, ...) __attribute__((noreturn)) __attribute__((format(printf, 1, 2))); 
//或者
extern void die(const char *format, ...) __attribute__((noreturn, format(printf, 1, 2)));
```
