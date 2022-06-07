###linux命令

- uname
```
uname
系统信息
	-s	内核名称（默认选项）
	-a	全部
	-p	CPU 信息
	-n	主机名
	-r	内核发行信息（版本号）
	-v	内核版本信息
```


- 安装samba
	sudo apt-get install samba
	修改配置文件
	/etc/samba/smb.conf
	测试配置
	testparm
	创建本地共享路径
	重启服务
	systemctl status smbd
	sudo systemctl restart smbd
	安装客户端
	sudo apt-get install smbclient
	测试连接-本地
	smbclient -L 127.0.0.1 -U user_name%password
	创建samba用户,首先本地存在该系统用户
	sudo smbpasswd -a user_name
	


### mount 
挂载windows共享文件夹
```
mount -t cifs -o username=XDYG,password=74108520,vers=3.0 //192.168.0.106/lyp_share  /home/lyp/yp_share
```


### readelf 
用于查看`.a`、`.so`、`.out`文件的符号信息
`readelf -s libvis.so`


### find 
find命令用来指定目录下查找文件，任何位于参数之前的字符串都将被视为欲查找的目录名。如果不设置任何参数，则find命令将在当前目录下查找子目录与文件。  
