## pragma  
预处理指令，作用是设定编译器的状态活着指示编译器完成一些特定动作


### pragma pack(n)  
设定结构体、联合体以及类成员变量以n字节方式对其  

- `#pragma pack(show)`  显式当前内存对齐的字节数   


```cpp
#pragma pack(push)  //保存对齐状态
#pragma pack(4) //设定为4字节对齐
struct test{
    char m1;
    double m4;
    int m3;
}


#pragma pack(pop)


```