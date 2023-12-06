## windows平台的服务   
- 打开系统服务管理  运行：services.msc  

### 首先进行service的程序编写  
参照"WinService.cpp"文件中的实现，将要需要服务执行的程序添加进框架中，注意设置服务的名称。 在本例中服务通过执行脚本实现调用目标程序，这样可方便替换实际运行的目标程序。  

### 通过sc.exe进行服务管理   
```shell
sc.exe create MyService binpath="D:\MyProj\Myservice.exe"  
sc.exe config MyService start= auto  
se.exe config MyService start= demand #手动  
se.exe config MyService start= disabled #禁用
sc.exe start MyService
sc.exe stop MyService
```


## Linux平台的服务  
- 首先准备好XXX.service服务文件  
```service
[Unit]
#服务描述
Description=xdp card http Service
#指定了在systemd在执行完那些target之后再启动该服务
After=network.target
 
[Service]
#定义Service的运行类型，一般是forking(后台运行)   
Type=forking
 
#定义systemctl start|stop|reload *.service 的执行方法（具体命令需要写绝对路径）
ExecStart=sudo bash /home/xxx/Desktop/xdpcard.sh --start
ExecReload=
ExecStop=sudo bash /home/xxx/Desktop/xdpcard.sh --stop
 
#创建私有的内存临时空间
PrivateTmp=True
 
[Install]
#多用户
WantedBy=multi-user.target
```  

- 通常服务中启动、停止、重启等命令都是通过脚本来实现，准备好一个运行脚本  
```shell
#!/bin/bash  
while true; 
do
    case $1 in
    --start)
        #执行目标程序
        exec sudo /home/xxx/xdpcard_server &
        exit 0
        ;;
    --stop)
        SERVICE_PID=`pgrep xdpcard_server`
        echo "Get PID:\n${SERVICE_PID}"
        if [ -z "$SERVICE_PID" ]; then
            exit 1
        fi
        SERVICE_NUM=`pgrep xdpcard_server | wc -l`
        INDEX=0
        while [[ $INDEX -lt $SERVICE_NUM ]]; do
            INDEX=$(expr "$INDEX" + 1)
            SERVICE_PID=`pgrep xdpcard_server | awk 'NR==1{print}'`
            echo "Kill ${INDEX} PID:${SERVICE_PID}"
            sudo kill -9 ${SERVICE_PID}
        done
        exit 0
        ;;
	esac
done
}
```

- 将服务文件放到指定目录下（通常为/usr/lib/systemd/system， 如没有system目录，创建即可）

- 通过systemctl来进行服务管理  
```
sudo systemctl daemon-reload
sudo systemctl enable XXX.service
sudo systemctl start XXX.service
```