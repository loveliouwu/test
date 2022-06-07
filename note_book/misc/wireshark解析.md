ISO五层模型

Frame 125172: 物理层数据帧概况。
Ethernet II, Src: 数据链路层以太网帧头部信息。
Internet Protocol Version 4, Src: 互联网层IP包头部信息。
Transmission Control Protocol, Src Port: 传输层数据段头部信息，此处是TCP协议。
Hypertext Transfer Protocol：应用层信息，此处是HTTP协议。
 

二、各层次分析
1、物理层的数据帧概况（Frame）

Frame 125172: 542 bytes on wire (4336 bits), 542 bytes captured (4336 bits) on interface \Device\NPF_{E0643354-5127-47B3-8346-56317724EE21}, id 0               #125172号帧，线路542字节，实际捕获542字节
Interface id: 0 (\Device\NPF_{E0643354-5127-47B3-8346-56317724EE21})         #接口id
Encapsulation type: Ethernet (1)                                                                            #封装类型
Arrival Time: Apr 17, 2020 12:00:41.047480000 中国标准时间                               #捕获日期和时间
[Time shift for this packet: 0.000000000 seconds]                                                #此数据包的时间偏移
Epoch Time: 1587096041.047480000 seconds                                                      #新纪元时间
[Time delta from previous captured frame: 0.000246000 seconds]                      #此包与前一包的时间间隔
[Time delta from previous displayed frame: 0.812755000 seconds]                     #此包与前一帧的时间间隔
[Time since reference or first frame: 135.245056000 seconds]                             #此包与第一帧的时间间隔
Frame Number: 125172                                                                                         #帧序号
Frame Length: 542 bytes (4336 bits)                                                                      #帧长度
Capture Length: 542 bytes (4336 bits)                                                                   #捕获长度
[Frame is marked: False]                                                                                         #此帧是否做了标记：否
[Frame is ignored: False]                                                                                         #此帧是否被忽略：否
[Protocols in frame: eth:ethertype:ip:tcp:http]                                                       #帧内封装的协议层次结构
[Coloring Rule Name: HTTP]                                                                                  #着色标记的协议名称
[Coloring Rule String: http || tcp.port == 80 || http2]                                            #着色规则显示的字符串
 

2、数据链路层以太网帧头部信息（Ethernet II）

Ethernet II, Src: IntelCor_28:34:26 (5c:87:9c:28:34:26), Dst: NewH3CTe_1c:68:01 (90:e7:10:1c:68:01)
Destination: NewH3CTe_1c:68:01 (90:e7:10:1c:68:01)              #目标MAC地址
Source: IntelCor_28:34:26 (5c:87:9c:28:34:26)                          #源MAC地址
Type: IPv4 (0x0800)                                                                  #类型：IPV4
 

3、互联网层IP包头部信息（Internet Protocol Version 4）

Internet Protocol Version 4, Src: 10.206.146.17, Dst: 10.206.142.12  #IPV4协议
0100 .... = Version: 4                                                                           #互联网协议IPv4
.... 0101 = Header Length: 20 bytes (5)                                              #IP包头部长度
Differentiated Services Field: 0x00 (DSCP: CS0, ECN: Not-ECT)         #差分服务字段
Total Length: 528                                                                                #IP包的总长度
Identification: 0xd80e (55310)                                                            #标志字段
Flags: 0x4000, Don't fragment                                                            #标记字段
Fragment offset: 0                                                                               #分的偏移量
Time to live: 128                                                                                  #生存期TTL
Protocol: TCP (6)                                                                                  #此包内封装的上层协议为TCP
Header checksum: 0x0000 [validation disabled]                                 #头部数据的校验和
[Header checksum status: Unverified]                                                 #头部数据的校验和：未经核实
Source: 10.206.146.17                                                                          #源IP地址
Destination: 10.206.142.12                                                                  #目标IP地址
 

4、传输层TCP数据段头部信息（Transmission Control Protocol）

Transmission Control Protocol, Src Port: 58447, Dst Port: 59992, Seq: 1, Ack: 1, Len: 488
Source Port: 58447                                                                          #源端口号
Destination Port: 59992                                                                   #目标端口号
[Stream index: 44]                                                                            #流索引
[TCP Segment Len: 488]                                                                  #TCP段长度
Sequence number: 1    (relative sequence number)                       #序列号（相对序列号）
Sequence number (raw): 2919587572                                            #序列号（原始序列号）
[Next sequence number: 489    (relative sequence number)]        #下一个序列号
Acknowledgment number: 1    (relative ack number)                    #确认序列号
Acknowledgment number (raw): 3188204174                               #确认序列号（原始）
0101 .... = Header Length: 20 bytes (5)                                          #头部长度
Flags: 0x018 (PSH, ACK)                                                                 #TCP标记字段
Window size value: 261                                                                  #流量控制的窗口大小
[Calculated window size: 66816]                                                    #计算窗口大小
[Window size scaling factor: 256]                                                  #窗口大小比例因子
Checksum: 0x37bc [unverified]                                                      #TCP数据段的校验和
[Checksum Status: Unverified]                                                       #TCP数据段的校验和状态
Urgent pointer: 0                                                                           #紧急指针
[SEQ/ACK analysis]                                                                        #序列/确认分析
[Timestamps]                                                                                 #时间戳
TCP payload (488 bytes)                                                               #TCP有效载荷
 

5、应用层协议（Hypertext Transfer Protocol）

GET /record/device/03e18244/2020-04-13/17-30-00.mp4 HTTP/1.1\r\n     #get请求地址
[Expert Info (Chat/Sequence): GET /record/device/03e18244/2020-04-13/17-30-00.mp4 HTTP/1.1\r\n]
Request Method: GET                                                                                     #请求方法
Request URI: /record/device/03e18244/2020-04-13/17-30-00.mp4             #请求URI
Request Version: HTTP/1.1                                                                             #http请求版本
Host: 10.206.142.12:59992\r\n                                                                       #主机
Connection: keep-alive\r\n                                                                            #连接：保持连接
Accept-Encoding: identity;q=1, *;q=0\r\n                                                     #接收的编码格式
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.132 Safari/537.36\r\n                                                                                            #浏览器及电脑信息
Accept: */*\r\n                                                                                                #接收
Referer: http://10.206.142.12:59992/record/device/03e18244/2020-04-13/17-30-00.mp4\r\n             #参照者url
Accept-Language: zh-CN,zh;q=0.9\r\n                                                          #接受语言：中文
Cookie: ZL_COOKIE=9e403cbd28ea02c1d12607599fc6b00e\r\n                  #cookie
Range: bytes=74022912-\r\n                                                                         #请求的数据量范围
[Full request URI: http://10.206.142.12:59992/record/device/03e18244/2020-04-13/17-30-00.mp4]   #完整的请求URI
[HTTP request 1/1]                                                                                         #请求次数