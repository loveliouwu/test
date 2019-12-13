#C++知识点查漏补缺

- ###关键字相关
	- `explicit`
	该关键字可以组织不应该允许的经过转换构造函数进行的隐式转换的发生，声明为explicit的构造函数不能在隐式转换中使用。
	
```
	class Test1
	{
	public:
	    Test1(int n)
	    {
		num=n;
	    }//普通构造函数
	private:
	    int num;
	};
	class Test2
	{
	public:
	    explicit Test2(int n)
	    {
		num=n;
	    }//explicit(显式)构造函数
	private:
	    int num;
	};
	int main()
	{
	    Test1 t1=12;//隐式调用其默认拷贝构造函数,成功
	    Test2 t2=12;//编译错误,不能隐式调用其构造函数
	    Test2 t2(12);//显式调用成功
	    return 0;
	}
```