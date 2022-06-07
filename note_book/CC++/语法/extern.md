- 被extern限定的函数或变量是extern类型的
- 被extern "C"修饰的变量和函数是按照C语言方式编译和链接的  
>extern "C" 的作用是让 C++ 编译器将 extern "C" 声明的代码当作 C 语言代码处理，可以避免 C++ 因符号修饰导致代码不能和C语言库中的符号进行链接的问题

## 使用  
```cpp
#ifdef __cplusplus
extern "C" {
#endif

void *memset();

#ifdef __cplusplus
}
#endif

```