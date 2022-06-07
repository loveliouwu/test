#define SWAP32(n)  ((n<<24)|(n&0x0000ff00)<<8|(n&0x00ff0000)>>8|(n>>24))    //字节大小端变换



####  宏定义中`#`号和`##`作用

##是连接符号，由两个井号组成，其功能是在带参数的宏定义中将两个子串(token)联接起来，从而形成一个新的子串。
但它不可以是第一个或者最后一个子串。所谓的子串(token)就是指编译器能够识别的最小语法单元。具体的定义在编译原理里有详尽的解释。
#符是把传递过来的参数当成字符串进行替代
假设程序中已经定义了这样一个带参数的宏：
#define PRINT( n ) printf( "token" #n " = %d", token##n )
同时又定义了二个整形变量：
int token9 = 9;
现在在主程序中以下面的方式调用这个宏：
PRINT( 9 );
那么在编译时，上面的这句话被扩展为：
printf( "token" "9" " = %d", token9 );
注意到在这个例子中，PRINT(9);中的这个”9”被原封不动的当成了一个字符串，与”token”连接在了一起，从而成为了token9。而#n也被”9”所替代。
可想而知，上面程序运行的结果就是在屏幕上打印出token9=9


#### 软中断

```C
void time_out(int sig)
{
        signal(SIGALRM, time_out);
        run = 0;
}
signal(SIGALRM, time_out);
printf("Now test 20 seconds encrypt ...\n");
i = 0;
alarm(20);
for (run = 1; run; i++)
        SM3((unsigned char *)"12324524alsdkf", 12, digest);

printf("SM3 digest times in 20 seconds: [%ld]\n", i);
```



### 零长度数组   
GUN C允许声明长度为零的数据，用于创建变长结构，只有当零长度数据结构是struct的最后一个成员的时候才有意义  
```C
typedef struct{
	int size;
	char string[0];
}vlen;

int main()
{
	vlen *line = (vlen *)malloc(sizeof(vlen) + 16); //其中sizeof(vlen)的大小只有size的大小
	memset(line->string,'a',16);

	vlen initvlen = {4, {'a', 'b', 'c', 'd'}};//另一种初始化方式
}
```

### 三目运算 操作数省略  
条件表达式中，条件真或者假是由表达式结果是否为零决定的，如下： 
```C
x = y ? y : z;
//如果y为非零，则x=y,此时y被执行了两次，如果修改该表达式如下： 
x = y ? : z;
//则第二次y就不必执行。 

//test.c
int print(int a)
{
	printf("a = %d\n", a);
	return a*2;
}
void main()
{
	int a = 2;
	a = (a>2) ? print(a) : 99;
	printf("after a = %d\n",a);
	//a = 2 //print函数中打印的数据
	//after a = 4;//主函数中打印的数据
}
```

### 结构体互相包含  
当两个结构体互相包含的时候，需要有一个先进行声明，另一个就可以包含了  
```C
struct sta;//仅声明  
struct stb{
	struct sta;
	int val;
};
struct sta{
	struct stb;
	int val;
}
```

### 枚举的不完全类型  
同样的枚举也可以先声明再实际定义    
```C
enum color_list;
enum color_list{BLACK, WHITE, BLUE};
```


### 内联函数  
被声明为内联函数的函数，在调用它的位置，代码会像宏一样被展开，通过关键字`inline`，可将函数声明为内嵌函数。  
```C
inline int halve(double x){
	return (x/2.0);
}
``` 
- 除非使用选项`-o`指明优化级别，否则不会有函数会被真正的、内嵌式的展开。   
- 如果设置函数的属性为`always_inline`，则不管是否优化都会被迫展开为内联函数。  


### goto 的另一种用法  
用标识地址来标记地址，将它保存在指针中，再用goto语句跳转到标记处，可通过&&操作符访问该地址。  
```C
#include<stdio.h>
int test()
{
	void *exit;//定义一个指针，用来存储标识地址
	int a = 5;
	if(a>0)
	{
		a--;
		exit = &&exit1;//通过 && 来赋值
	}
	if(a>5)
	{
		a--;
		exit = &&exit2;
	}
	goto *exit;//通过 *exit 来跳转到标记处
exit1:
	printf("exit1\n");
exit2:
	printf("exit2\n");
}

```

### switch case 的另一种用法   
当case是一个范围的话，可以使用特别的写法来省略：
```
int a = 4;
switch(a)
{
	case 0:
	case 1:
		printf("0 or 1\n");
		break;
	case 2 ... 5:
		printf("2 ... 5\n");
		break;
	default:
		printf("defalut\n");
}

//'a'  'z'
case 'a' ... 'm':
case 'A' ... 'F':
```

### typedef 名字创建  
关键字typedef可用于为表达式的数据类型创建名字，定义的名字可用来声明变量或将变量强制类型转换为该表达式的数据类型。
```C
#define swap(a,b) \
        {{typedef _tmp = a;\
        _tmp temp = a;\
        a = b;\
        b = temp;}}

typedef smallread=0.0f;
smallread real1;
```

### 静态变量存储
- `static int a = 9;`  
定义的静态存储变量无论是做全局变量或者局部变量，定义和初始化在程序编译时进行，作为局部变量，调用函数结束时，静态存储变量不消失并且保存原值。   

### 读写字符串   
在C语言中，最简单的IO函数是`getchar()`,`putchar()`,两者的功能分别是从键盘输入一个字符串和向显示屏显示一个字符串。  
```C
int getchar(void);
int putchar(int c);  
//失败时返回EOF  
```  
用于读写字符串的函数是`gets()`,`puts()`,这两个函数允许程序从控制台读取字符串和向控制台写字符串。  
```C
char *gets(char *string);//string是接受用户键入串的字符数组，函数返回值也是同一个string，但后打印出它的长度。   
int puts(const char *string);
```  

### 枚举  
```C
enum money{penny, nickel, dime, quatrer, half_dollar, dollar};  
enum money coin;
coin = penny;
```


### printf 回到行首
```c
	printf("%d\n",i);
	printf("\033[1A");
	printf("\033[K");
	... do something

```