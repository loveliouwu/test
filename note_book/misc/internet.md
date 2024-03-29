# 网络协议
- --

### 使用TCP的协议：

> 
| 协议  |  |
| - | - |
| HTTP | 超文本传输协议
| FTP | 文件传输协议
|SMTP|简单邮件传输协议
| Telnet | *远程登录的标准协议*

### 使用UDP的协议：
>
| 协议 |  |
| - | ----- |
| DHCP |动态主机配置协议
| DNS | 域名系统 注：*域名解析成ip地址*
| SNMP | 简单网络管理协议
| TFTP | 简单文件传输协议

## IP 网段 子网掩码
通过IP和子网掩码可以获取该IP所在的网段，即通过与操作：
如：
IP:192.168.1.106
子网掩码:255.255.255.0
则网段为:192.168.1.0
即该网段下最多只能有：256-2台主机，其中0为该网段，1为广播，网段和广播无法分配IP 
在同一个网段下的数据不需要路由器进行转发，不同网段的数据交给路由器进行转发

## 路由器 工作在网络层

数据流向：  
物理层收到数据后>>发送给数据链路层>>路由器接手工作：
1、根据转发信息表更新IP头
2、....
工作结束>>发送给链路层封装>>发送给物理层

每个路由器中都有一个路由表和转发表（FIB）。路由表用来决策路由，FIB用来转发分组。  
路由表：用来保存路由信息，通常由路由协议和路由管理模块维护。  
路由有三类：  
1、链路层协议发现的路由，即直连路由。   
2、静态路由。  
3、动态路由协议发现的路由。   

转发表：  
基于路由生成的，转发表中每条转发项都指明分组到某个网段或者主机应该通过路由器的哪个物理接口发送，然后就可以到达该路径的下一个路由器，或者不在经过别的路由器而传送到直接相连的网络中的目的主机。   
高性能路由器转发表通常都用硬件来实现，有利于高速查找。   
CAM、TCAM是一种特殊的存储器，无论表大小如何，都会在一个时钟周期内检索出地址  



## MTU   
在TCPIP协议中，以太网的数据帧在链路层，IP包在网络层，TCP或UDP在传输层，TCP或UDP中的数据来自应用层，他们的关系为：   
数据帧{IP包{TCP或UDP包{Data}}}    
 
在应用程序中我们用到的Data的长度最大是多少，直接取决于底层的限制：   
1、在链路层，由以太网的物理特性决定了数据帧的长度（46+18）--（1500+18），其中18是数据帧的头和尾，也就是说数据帧的内容最大位1500（不包括帧头和帧尾），即MTU位1500.   
2、在网络层，因为IP包的首部要占用20字节，所以这的MTU为1500-20 = 1480   
3、在传输层，对于UDP包的首部要占用8字节，所以这的MTU为1480 - 8 = 1472   
所以在应用层，Data最大长度为1472，当我们的UDP包中的数据多于MTU时，发送方的IP层需要分片进行传输，而在接收方IP层则需要进行数据报重组，由于UDP是不可靠的传输协议，如果分片导致数据重组失败，将导致UDP数据包被丢弃。因此在不同的局域网环境下，UDP的数据最大为1472字节最好，但在网络编程中，Internet中的路由器可能有设置成不同的值，Internet上的标准MTU为576，所以Internel的UDP编程时数据长度最好在576-20-8=548字节以内。


MTU对我们的UDP编程很重要，那如何查看路由的MTU值呢？ 　　
对于windows OS: ping -f -l 　　如：ping -f -l 1472 192.168.0.1 　　
如果提示：Packets needs to be fragmented but DF set. 　　则表明MTU小于1500，不断改小data_length值，可以最终测算出gateway的MTU值； 　　
对于linux OS: ping -c -M do -s 　　如： ping -c 1 -M do -s 1472 192.168.0.1 　　
如果提示 Frag needed and DF set…… 　　则表明MTU小于1500，可以再测以推算gateway的MTU。   

## 滑动窗口   
发送方拥有着一个窗口，这个窗口里面装着可以发送的帧：
1、它需要暂时存储这些帧，因为这些帧可能被重传。  
2、当这个窗口接收到确认帧后，它就会向前移动。  
接收方拥有一个可以接受帧的窗口：   
1、它要为将要到来的帧预留空间。   
2、这个窗口随着按顺序到来的帧而逐渐向前移动  
一开始的时候，接受方可以接受一个帧，发送方之后发送了一个帧，接收方接受了这个帧之后，可以向后滑动一个单位，发送方得到接收方回复给它的确认帧ack之后，继续向后滑动一个窗口，准备发送下一个帧。  
