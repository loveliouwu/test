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


- STL list小计 删除嵌套链表
```
typedef struct session {
	int a;
}*pSession;

typedef struct sessionkey {
	int b;
}*pSessionKey;

typedef list<pSession> listSession;
typedef list<pSessionKey> listSessionKey;
typedef struct VHSMst {					//虚拟机数据结构，通过客户端提供的指针访问时，需要先验证Rodam
	unsigned long	Rodam;					//必须放在首部
	void* VHSMDevice;				//虚拟密码机对象
	listSession		MySessionList;
	listSessionKey		MySessionKeyList;
}VHSMst, * pVHSMst;


typedef list<VHSMst> List;
List vlist;
int main()
{
	VHSMst list1;
	pSession pse;
	pSessionKey pskey;
	list1.VHSMDevice = nullptr;
	list1.Rodam = 2;
	vlist.push_back(list1);

	VHSMst list2;
	list2 = vlist.back();
	//List.pop_back();
	list1.VHSMDevice = nullptr;
	list1.Rodam = 2;
	List::iterator ite;
	for (ite = vlist.begin(); ite != vlist.end(); ite++)
	{
		VHSMst tmp_list = *ite;
		if (tmp_list.Rodam == (unsigned long)2)
			ite = vlist.erase(ite);
	}
	//vlist.remove(list1);
	printf("%d\n", list2.Rodam);

	int c;
	scanf("%c",&c);

	return 0;
}
```