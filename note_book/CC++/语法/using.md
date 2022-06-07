## 一次using声明一次只能引用命名空间的一个成员  
`using namespace_name::name`   

## 在构造函数的using声明  【c++11】  
在 C++11 中，派生类能够重用其直接基类定义的构造函数
class Derived::Base{
    public:
        using Base::Base;
        ...
} 


### 使用
尽量少使用 using 指示
    using namespace std;
应该多使用 using 声明
    int x;
    std::cin >> x ;
    std::cout << x << std::endl;
或者
    using std::cin;
    using std::cout;
    using std::endl;
    int x;
    cin >> x;
    cout << x << endl;

