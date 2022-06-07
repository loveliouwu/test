## 函数定义   

#### 创建套接字  
```c
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
/*
成功时返回文件描述符，失败时返回-1
domain: 套接字中使用的协议族（Protocol Family）
type: 套接字数据传输的类型信息
protocol: 计算机间通信中使用的协议信息
*/

int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);
//成功时返回0，失败时返回-1

int listen(int sockfd, int backlog);
//成功时返回0，失败时返回-1
//sock: 希望进入等待连接请求状态的套接字文件描述符，传递的描述符套接字参数称为服务端套接字
//backlog: 连接请求等待队列的长度，若为5，则队列长度为5，表示最多使5个连接请求进入队列
//只有调用了listen函数，客户端才能进入可发出连接请求的状态，换言之，这时客户端才能调用connect函数。


int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
//成功时返回文件描述符，失败时返回-1
//sock: 服务端套接字的文件描述符
//addr: 保存发起连接请求的客户端地址信息的变量地址值
//addrlen: 的第二个参数addr结构体的长度，但是存放有长度的变量地址。
//函数受理连接请求队列中待处理的客户端连接请求，函数调用成功后，
```    

#### 结构体和值  
```c
struct in_addr
{
    in_addr_t s_addr; //32位IPV4地址
};
struct sockaddr_in
{
    sa_family_t sin_family;  //地址族（Address Family）
    uint16_t sin_port;       //16 位 TCP/UDP 端口号
    struct in_addr sin_addr; //32位 IP 地址
    char sin_zero[8];        //不使用
}; 
``` 

- 成员 sin_family

每种协议适用的地址族不同，比如，IPV4 使用 4 字节的地址族，IPV6 使用 16 字节的地址族。
> 地址族

| 地址族（Address Family） | 含义                               |
| ------------------------ | ---------------------------------- |
| AF_INET                  | IPV4用的地址族                     |
| AF_INET6                 | IPV6用的地址族                     |
| AF_LOCAL                 | 本地通信中采用的 Unix 协议的地址族 |

AF_LOACL 只是为了说明具有多种地址族而添加的。  
- 成员 sin_port

  该成员保存 16 位端口号，重点在于，它以网络字节序保存。

- 成员 sin_addr

  该成员保存 32 为IP地址信息，且也以网络字节序保存

- 成员 sin_zero

  无特殊含义。只是为结构体 sockaddr_in 结构体变量地址值将以如下方式传递给 bind 函数。

#### 协议族 （Protocol Family）  
通过 socket 函数的第一个参数传递套接字中使用的协议分类信息，此协议分类信息成为协议族，可分为如下几类：  

> 头文件 `sys/socket.h` 中声明的协议族  
> 

| 名称      | 协议族               |
| --------- | -------------------- |
| PF_INET   | IPV4 互联网协议族    |
| PF_INET6  | IPV6 互联网协议族    |
| PF_LOCAL  | 本地通信 Unix 协议族 |
| PF_PACKET | 底层套接字的协议族   |
| PF_IPX    | IPX Novel 协议族     |

#### 套接字类型（Type）

套接字类型指的是套接字的数据传输方式，是通过 socket 函数的第二个参数进行传递，只有这样才能决定创建的套接字的数据传输方式。**已经通过第一个参数传递了协议族信息，为什么还要决定数据传输方式？问题就在于，决定了协议族并不能同时决定数据传输方式。换言之， socket 函数的第一个参数 PF_INET 协议族中也存在多种数据传输方式。**


#### 套接字类型1：面向连接的套接字（SOCK_STREAM）

如果 socket 函数的第二个参数传递`SOCK_STREAM`，将创建面向连接的套接字。

传输方式特征整理如下：

- 传输过程中数据不会消失
- 按序传输数据
- 传输的数据不存在数据边界（Boundary）

> 传输数据的计算机通过调用3次 write 函数传递了 100 字节的数据，但是接受数据的计算机仅仅通过调用 1 次 read 函数调用就接受了全部 100 个字节。

收发数据的套接字内部有缓冲（buffer），简言之就是字节数组。只要不超过数组容量，那么数据填满缓冲后过 1 次 read 函数的调用就可以读取全部，也有可能调用多次来完成读取。

**套接字缓冲已满是否意味着数据丢失？**

> 答：缓冲并不总是满的。如果读取速度比数据传入过来的速度慢，则缓冲可能被填满，但是这时也不会丢失数据，因为传输套接字此时会停止数据传输，所以面向连接的套接字不会发生数据丢失。  

#### 面向消息的套接字（SOCK_DGRAM）

如果 socket 函数的第二个参数传递`SOCK_DGRAM`，则将创建面向消息的套接字。面向消息的套接字可以比喻成高速移动的摩托车队。特点如下：

- 强调快速传输而非传输有序
- 传输的数据可能丢失也可能损毁
- 传输的数据有边界
- 限制每次传输数据的大小

面向消息的套接字比面向连接的套接字更具有传输速度，但可能丢失。  

```c
//创建TCP套接字  
int tcp_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);  
//创建UDP套接字  
int udp_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
```

#### sockaddr 和 sockaddr_in 的区别和关系  
```c
struct sockaddr {  
     unsigned short sa_family;
　　 char sa_data[14];                  
　　 }; 
//sa_family是通信类型，最常用的值是 "AF_INET"
//sa_data14字节，包含套接字中的目标地址和端口信息
struct sockaddr_in { 
　　 short int sin_family;
　　 unsigned short int sin_port; 
     struct in_addr sin_addr;
     unsigned char sin_zero[8];
}
struct in_addr { 
    unsigned long s_addr;
}
//sin_port和sin_addr都必须是NBO
//一般可视化的数字都是HBO（本机字节顺序）
```
函数中的参数输入类型事sockaddr，但我们通常使用sockaddr_in，通过类型强制转换再传递给函数。  
sockaddr_in结构体，struct sockaddr_in中的in表示internet，就是网络地址，这只是我们比较常用的地址结构，属于AF_INET地址族。  

> sockaddr 和 sockaddr_in 的相互关系  
一般先把sockaddr_in变量赋值后，强制类型转换后传入用sockaddr做参数的函数  
即：  **sockaddr_in用于socket定义和赋值，sockaddr用于函数参数**  
```c  
//sockaddr_in结构体变量的基本配置  
struct sockaddr_in ina;
bzero(&ina, sizeof(ina)); 
ina.sin_family = AF_INET;
ina.sin_port = htons(23);
ina.sin_addr.s_addr = inet_addr("127.0.0.1");  


//服务端创建socket的例子：  
//配置源socket，即本机地址  
struct sockaddr_in cliaddr;  
bzero(&cliaddr, sizeof(cliaddr))；
cliaddr.sin_family = AF_INET;
//通常源地址和端口都是随机的  
cliaddr.sin_addr.s_addr = htons(INADDR_ANY);
cliaddr.sin_port = htons(0);
```  

#### 网络字节序和本机字节序转换函数  
> 网络字节顺序NBO，结构体sin_port和sin_addr都必须事NBO  
> 本机字节顺序HBO，一般可视化的数字都是HBO 
```c
#include<arpa/inet.h>
in_addr_t inet_addr(const char *string); //将字符串点数格式地址转化成无符号长整型（unsigned long s_addr s_addr;）
int inet_aton(const char *string, struct in_addr *addr)    //将字符串点数格式地址转化成NBO 时评频率更高
char *inet_ntoa (struct in_addr adr);     //将NBO地址转化成字符串点数格式
unsigned short htons(unsigned short);    //"Host to Network Short"
unsigned long htonl(unsigned long);    //"Host to Network Long"
unsigned short ntohs(unsigned short)    //"Network to Host Short"
unsigned long ntohl(unsigned long)    //"Network to Host Long"
//常用的是htons(),inet_addr()正好对应结构体的端口类型和地址类型 
//htons 的h代表主机（host）的字节序。n代表网络字节序
//尾字母标识short或者long
```
> 3种给socket赋值**地址**的方法   
```c
//1 
char server_addr_string[] = "192.168.1.1";
inet_aton(server_addr_string, &myaddr.sin_addr);  

//2 **更加常用**
myaddr.sin_addr.s_addr = inet_addr()
```

> 综上，网络地址初始化方法常见的如下：
```c
struct sockaddr_in addr;
char *serv_ip = "127.0.0.1";
char *serv_port = "8802";
memset(&addr, 0, sizeof(addr));

addr.sin_family = AF_INET;
addr.sin_addr.s_addr = inet_addr(serv_ip);
addr.sin_port = htons(atoi(serv_port));

``` 


## UDP  
UDP是一种不可靠的数据传输方式，性能比TCP高。   
UDP中的服务端和客户端不像TCP那样在链接状态下交换数据，因此与TCP不同，无需经过连接过程，也就是说，不必调用TCP连接过程中调用的listen和accept函数。在TCP中套接字之间应该是一对一的关系，若要向10个客户端提供服务，除了守门的服务端套接字外，还需要10个服务器套接字，但在UDP中，不管是服务器端还是客户端都只需要一个套接字。  

### UDP的I/O函数  
TCP套接字创建好之后，传输数据时无需加上地址信息，因为TCP套接字将保持与对方套接字的连接，TCP套接字知道目标地址信息，但UDP套接字不会保持连接状态，因此每次传输数据时都需要添加目标的地址信息  
```c
#include<sys/socket.h> 
ssize_t sendto(int sock, void *buff, size_t nbytes, int flags, struct sockaddr *to, socklen_t addrlen);  
/*
成功时返回传输的字节数，失败时返回-1
sock: 用于传输数据的UDP套接字  
buff: 保存待传输的数据长度，以字节为单位
nbutes: 待传输的数据长度，以字节为单位 
flags: 可选项参数，若没有则传递0
to: 存有目标地址的sockaddr结构体变量的地址值  
addrlen: 传递给参数to的地址值的结构体变量长度
*/

ssize_t recvfrom(int sock, void *buff, size_t nbytes, int flags, struct sockaddr *from, socklen_t *addrlen);
/*
成功时返回传输的字节数，失败返回 -1
sock: 用于传输数据的 UDP 套接字
buff: 保存待传输数据的缓冲地址值
nbytes: 待传输的数据长度，以字节为单位
flags: 可选项参数，若没有则传递 0
from: 存有发送端地址信息的 sockaddr 结构体变量的地址值
addrlen: 保存参数 from 的结构体变量长度的变量地址值。
*/
```   

- UDP调用顺序一般例子 
  - client  
  socket >> sendto >> recvfrom >>close
  socket >> connect >> write >> read >> close
  - server 
  socket >> bind >> recvfrom >> sendto >> close


### 优雅的断开socket连接  
linux和windows的closesocket函数意味着完全断开连接，完全断开不仅指无法传输数据，而且不能接收数据。但这会造成半断开。  
- shutdown函数，关闭socket的输入或者输出流  
```c
#include<sys/socket.h>
int shutdown(int sock, int howto);
/*
成功返回0，失败放回-1
sock: 需要断开套接字文件描述符  
howto: 传递断开方式信息
*/
```  
**howto**值有`SHUT_RD`断开输入流、`SHUT_WR`断开输出流、`SHUT_RDWR同时断开I/O流`  
若向shutdown的第二个参数传递SHUT_RD则断开输入流，套接字无法接收数据，而且无法调用相关函数。


### DNS  
**gethostbyname**这个函数使用方便，只要传递字符串，就可以返回域名对应的IP地址。只是返回时，地址信息装入 hostent 结构体。
```c
#include <netdb.h>
struct hostent *gethostbyname(const char *hostname);
/*
成功时返回 hostent 结构体地址，失败时返回 NULL 指针
*/

struct hostent
{
    char *h_name;       /* Official name of host.  */
    char **h_aliases;   /* Alias list.  */
    int h_addrtype;     /* Host address type.  */
    int h_length;       /* Length of address.  */
    char **h_addr_list; /* List of addresses from name server.  */
};
```
- h_name：该变量中存有官方域名（Official domain name）。官方域名代表某一主页，但实际上，一些著名公司的域名并没有用官方域名注册。
- h_aliases：可以通过多个域名访问同一主页。同一IP可以绑定多个域名，因此，除官方域名外还可以指定其他域名。这些信息可以通过 h_aliases 获得。
- h_addrtype：gethostbyname 函数不仅支持 IPV4 还支持 IPV6 。因此可以通过此变量获取保存在 h_addr_list 的IP地址族信息。若是 IPV4 ，则此变量中存有 AF_INET。
- h_length：保存IP地址长度。若是 IPV4 地址，因为是 4 个字节，则保存4；IPV6 时，因为是 16 个字节，故保存 16
- h_addr_list：这个是最重要的的成员。通过此变量以整数形式保存域名相对应的IP地址。另外，用户比较多的网站有可能分配多个IP地址给同一个域名，利用多个服务器做负载均衡，。此时可以通过此变量获取IP地址信息。    

- 利用IP获取域名  
```c
#include<netdb.h>
struct hostent * gethostbyaddr(const char *addr, socklen_t len, int family);
/*
成功时返回hostent结构体变量地址值，失败时返回NULL指针
*/
```  

### 套接字可选项  

| 协议层 | 选项名 | 读取 | 设置 |
| :----: | :----: |:--: | :--: |
| SOL_SOCKET  |     SO_SNDBUF     |  O   |  O   |
| SOL_SOCKET  |     SO_RCVBUF     |  O   |  O   |
| SOL_SOCKET  |   SO_REUSEADDR    |  O   |  O   |
| SOL_SOCKET  |   SO_KEEPALIVE    |  O   |  O   |
| SOL_SOCKET  |   SO_BROADCAST    |  O   |  O   |
| SOL_SOCKET  |   SO_DONTROUTE    |  O   |  O   |
| SOL_SOCKET  |   SO_OOBINLINE    |  O   |  O   |
| SOL_SOCKET  |     SO_ERROR      |  O   |  X   |
| SOL_SOCKET  |      SO_TYPE      |  O   |  X   |
| IPPROTO_IP  |      IP_TOS       |  O   |  O   |
| IPPROTO_IP  |      IP_TTL       |  O   |  O   |
| IPPROTO_IP  | IP_MULTICAST_TTL  |  O   |  O   |
| IPPROTO_IP  | IP_MULTICAST_LOOP |  O   |  O   |
| IPPROTO_IP  |  IP_MULTICAST_IF  |  O   |  O   |
| IPPROTO_TCP |   TCP_KEEPALIVE   |  O   |  O   |
| IPPROTO_TCP |    TCP_NODELAY    |  O   |  O   |
| IPPROTO_TCP |    TCP_MAXSEG     |  O   |  O   |   

- IPPROTO_IP 可选项是IP协议相关事项  
- IPPROTO_TCP 层可选项是TCP协议相关事项  
- SOL_SOCKET 层是套接字的通用可选项   

#### `getsockopt` & `setsockopt`  
可选项的读取和设置通过以下两个函数来完成  
```c
#include<sys/socket.h> 
int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen);   
/*
成功时返回0，失败时返回-1
sock: 用于查看选项套接字文件描述符 
level: 要查看的可选协议层  
optname: 要查看的可选项 
optval: 保存查看结果的缓冲地址值  
optlen: 向第四个参数传递的缓冲大小，调用函数后，改变量中保存返回的可选项信息字节数
*/
```  

```c
#include <sys/socket.h>

int setsockopt(int sock, int level, int optname, const void *optval, socklen_t optlen);
/*
成功时返回 0 ，失败时返回 -1
sock: 用于更改选项套接字文件描述符
level: 要更改的可选项协议层
optname: 要更改的可选项名
optval: 保存更改结果的缓冲地址值
optlen: 向第四个参数传递的缓冲大小。调用函数候，该变量中保存通过第四个参数返回的可选项信息的字节数。
*/
```  

- demo例子 
```c
int tcp_sock,udp_sock;
int sock_type;
socklen_t optlen;
int state;

optlen = sizeof(sock_type);
tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
printf("SOCK_STREAM: %d\n", SOCK_STREAM);
printf("SOCK_DGRAM: %d\n", SOCK_DGRAM);
state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void *)&sock_type, &optlen);
if (state)
    error_handling("getsockopt() error");
printf("Socket type one: %d \n", sock_type);

state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void *)&sock_type, &optlen);
if (state)
    error_handling("getsockopt() error");
printf("Socket type two: %d \n", sock_type);
```  
结果： 
```
SOCK_STREAM: 1
SOCK_DGRAM: 2
Socket type one: 1
Socket type two: 2
```  

#### `SO_SNDBUF` & `SO_RCVBUF` 
创建套接字的同时会生成I/O缓冲  
```c
int snd_buf,rcv_buf,state;
state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, &len);
if (state)
    error_handling("getsockopt() error");

len = sizeof(rcv_buf);
state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, &len);
if (state)
    error_handling("getsockopt() error");

printf("Input buffer size: %d \n", rcv_buf);
printf("Output buffer size: %d \n", snd_buf);
``` 

#### `SO_REUSEADDR` 
##### 发生地址分配错误（Binding Error）  
> 服务器端和客户端都已经建立连接的状态下，向服务器控制台输入CTRL+C，强制关闭服务端，如果用这种方式终止程序，如果用同一端口再次运行服务端，就会输出[bind() error]消息，并且无法再次运行，但是在这种情况下，再过大约3分钟就可以重新运行服务端。  

##### `Time-wait`状态  
无论是服务端还是客户端都要经过一段时间的time-wait过程，先断开连接的套接字必然会经过Time-wait过程。  
为了解决time-wait，可以在套接字的可选项中更改SO_REUSEADDR的状态，适当调整该参数，可将Time-wait状态下的套接字端口号重新分配给新的套接字。 
```c
optlen = sizeof(option);
option = TRUE;
setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR,(void *)&option,optlen);
``` 


