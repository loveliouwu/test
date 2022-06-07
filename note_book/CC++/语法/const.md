### const

```cpp
class A{
    private: 
        const int a;    //常对象成员，只能在初始化列表赋值
    public:
        A(){};
        A(int x) : a(x)  {};  //初始化列表，初始化常成员对象

        int getValue();
        int getValue() const; //常成员函数，不得修改类中任何数据成员的值

}

void function()
{
    A a; 
    const A a1; //常对象，只能调用常成员函数，更新常成员变量
    const A *p = &a;
    const A &PP = a;

    char str[] = "hello";
    const char * const pstr = str;//常指针，指向字符数组常量    
}

```