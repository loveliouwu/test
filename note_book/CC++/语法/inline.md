## 内联函数  
 
### 特征  
- 相当于把内联函数里面的内容写在调用内联函数处
- 相当于不用执行进入函数的步骤，直接执行函数体  
- 相当于宏，却比宏多了类型检查，真正具有函数特性  
- 不能包含循环、递归、switch等复杂操作   
- 在类声明中定义的函数，除了虚函数的其他函数都会自动隐式的当作内联函数   


### 使用 
```cpp

inline int function(int first,int seconde, ...);

class A{
    int doa() { return 0; } //隐式内联
}

class B{
    int dob();
}
inline int B::dob() { return 0; } //显式内敛

```


### 编译器对inline函数的处理步骤  
1、将inline函数体复制到inline函数调用点处  
2、为所用inline函数中的局部变量分配内存空间
3、将inline函数的输入参数和返回值映射到调用方法的局部变量空间中
4、如果inline有多个返回点，将其转变为inline函数代码块末尾的分支（使用goto） 
