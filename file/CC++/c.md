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
