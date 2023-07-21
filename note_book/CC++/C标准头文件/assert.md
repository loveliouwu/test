## assert.h 常用于验证程序做出的假设，并在假设为假时输出诊断消息   

### 在头文件中定义的唯一的函数 
` void assert(int expression) ` 这实际上是一个宏，不是一个函数，可用于在C程序中添加判断    

### 注意不要在断言中放需要执行的代码  
assert函数只有在debug中才能执行，在编译release版本时不会被执行   

```C
//1 
int value = get_value();
assert(100>value&&value>-100);  //用于非预期错误  


```