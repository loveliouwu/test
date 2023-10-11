1.前言  
nmcli 是 NetworkManager 的提供的命令，使用 nmcli 命令时，必须确保 NetworkManager 为运行状态。nm 代表 NetworkManager，cli 代表 Command-Line 命令行。
```
查看状态：    systemctl status NetworkManager
启动：       systemctl start NetworkManager
重启：       systemctl restart NetworkManager
关闭：       systemctl stop NetworkManager
是否开机启动： systemctl is-enabled NetworkManager
开机启动：    systemctl enable NetworkManager
禁止开机启动： systemctl disable NetworkManager
```


2. nmcli networking  
显示 NetworkManager 是否接管网络设置：
```
nmcli networking
```
networking 可以简写为 n、ne、net、netw…… 所以以上命令可以简写为：
```
nmcli n
```
查看网络连接状态：
```
nmcli n connectivity
```
网络连接状态共有五种：full、limited（连网，但无法上网）、portal（连网，但需要认证登录后才能上网）、none（没连网）和 unknown。

connectivity 可以简写为 c，所以以上命令可简写为：
```
nmcli n c
```
开启网络连接：
```
nmcli n on
```
关闭网络连接：
```
nmcli n off
```
3. nmcli general  
显示系统网络状态：
```
nmcli general status
```

显示连接客户端主机名
```
nmcli g hostname # 或
nmcli g h
```

若修改主机名，需要重启 NetworkManager。

4.nmcli connection  
显示所有网络连接的信息：
```
nmcli connection show
```
connection 可以简写为 c、co、con、conn……

show 是 connection 的默认项，可以省略不写。所以，以上命令可简写为：
```
nmcli c
```
nmcli connection show 有一个 -active 参数，可以只显示当前启动的连接：
```
nmcli c s --active  # 或
nmcli c s -a        # 因为 show 后面有参数项，所以此时的 show 不能省
```

显示某一特定连接的详细信息（以 ens33 为例）：
```
nmcli c s ens33
```
启动指定连接：
```
nmcli c up ens33
```
如果 ens33 本来就出于连接状态，那此命令会重启 ens33。
关闭指定连接：
```
nmcli c down ens33
```
关闭连接后，执行 nmcli c 命令， DEVICE 项将显示为 – 。
修改连接：
```
nmcli c modify ens33  [ + | - ]选项 选项值  # 或
nmcli c m ens33  [ + | - ]选项 选项值
```
下面给出常用修改示例：
```
nmcli c m ens33 ipv4.address 192.168.80.10/24  # 修改 IP 地址和子网掩码
nmcli c m ens33 ipv4.method manual             # 修改为静态配置，默认是 auto
nmcli c m ens33 ipv4.gateway 192.168.80.2      # 修改默认网关
nmcli c m ens33 ipv4.dns 192.168.80.2          # 修改 DNS
nmcli c m ens33 +ipv4.dns 114.114.114.114      # 添加一个 DNS
nmcli c m ens33 ipv6.method ignored            # 将 IPv6 禁用
nmcli c m ens33 connection.autoconnect yes     # 开机启动
```
注意，必须先修改 ipv4.address，然后才能修改 ipv4.method！
用空引号""代替选项的值，可将选项设回默认值（以 ipv4.method 为例）：
```
nmcli c m ens33 ipv4.method ""
```
选项有很多，详细信息可以通过以下命令查看：
```
man 5 nm-settings-nmcli
```
主要可以看里面 connection setting 和 ipv4 setting 部分。
新增连接：
```
nmcli c add tyep 连接类型 选项 选项值  # 或
nmcli c a tyep 连接类型 选项 选项值
```
type 为必选项，我们通常用到的是 802-3-ethernet（别名 ethernet）。
选项与前面介绍的“修改连接”中的选项一样，其中重要的几个选项：
connection.id（别名 con-name）：连接名；
connection.interface-name（别名 ifname）：设备名；
connection.autoconnect（别名 autoconnect）：是否开机自启，其值只能是 yes 或 no，默认 yes。
下面给出一个示例：
```
nmcli c a type ethernet con-name ens36 ifname ens36
```
删除指定连接：
```
nmcli c delete ens33  # 或
nmcli c de ens33      # delete 不可简写为 d，否则与 down 冲突，但可以简写为 de
```
重载所有连接的配置文件：
```
nmcli c reload  # 或
nmcli c r
```
重载某一指定连接的配置文件：
```
nmcli c load ifcfg-ens33  # 或
nmcli c l ifcfg-ens33    
```
网络配置文件默认保存在 /etc/sysconfig/network-scripts/ 路径下，如果配置文件在其它位置，则需要填写完整路径。
网络配置文件的命名方式就是ifcfg-连接名，例如 ens33 的配置文件名为ifcfg-ens33。
网络配置文件说明：
```
TYPE=Ethernet           # 以太网
PROXY_METHOD=none
BROWSER_ONLY=no
BOOTPROTO=none          # 静态配置，等同于 ipv4.method manual
DEFROUTE=yes
IPV4_FAILURE_FATAL=no
IPV6INIT=no
NAME=ens33              # 连接名
UUID=16862343-b55e-4248-b05d-a3ea662a84a4
DEVICE=ens33            # 设备名
ONBOOT=yes              # 开机自启
IPADDR=192.168.80.10    # IP 地址
PREFIX=24               # 子网掩码
GATEWAY=192.168.80.2    # 网关
DNS1=192.168.80.2       # DNS1
DNS2=114.114.114.114    # DNS2
```
建议使用 nmcli 命令来设置网络参数，不要直接修改此文件。

5. nmcli device  
显示所有网络接口设备的状态：
```
nmcli device status
```
device 可以简写为 d、de、dev……
status 是 device 的默认项，可以省略不写。所以，以上命令可简写为：
```
nmcli d
```
显示所有设备的详细信息：
```
nmcli d show  # 或
nmcli d sh         # show 不可简写为 s，否则与 status 冲突，但可以简写为 sh
```
显示某一特定设备的详细信息：
```
nmcli d sh ens33
```
连接设备：
```
nmcli d connect ens33  # 或
nmcli d c ens33  
```
如果 ens33 本来就出于连接状态，那此命令会重启 ens33。

断开设备：
```
nmcli d disconnect ens33  # 或
nmcli d d ens33  
```
更新设备信息：
```
nmcli d reapply ens33  # 或
nmcli d r ens33  
```
只有在设备处于连接状态，才可以更新设备。
更新设备相当于重启连接。