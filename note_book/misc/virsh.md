# virsh 基本命令

### 启动虚拟机  
```shell
virsh define /etc/libvirt/qemu/node3.xml
virsh start node3
```

### 查看启动的虚拟机列表
```shell
- virsh list  
```

### virsh start vm_name   启动名字为vm_name的非活动虚拟机   

### virsh create x.xml		创建虚拟机 

### virsh suspend x 	暂停虚拟机，x可以为虚拟机名字，也可以为虚拟机id、uuid  

### virsh resume x	启动暂停的虚拟机   

### virsh shutdown  x
 
### virsh destroy x 

### virsh dominfo x

### virsh domname 2  

### virsh domid/domuuid/domstate/dompxml/    

### virsh  setmem x 512000 给不活动的虚拟机设置内存大小   

### virsh autostart x  设置虚拟机开机自启动  

### virsh autostart --disable x 关闭开机自启动   

### 创建新磁盘   
- qemu-img create -f qcow2 -o size=9G /opt/v_os/test.qcow2   


### 查看磁盘情况  
- qemu-img info ~/vos/qemu_lyp_0.20G

### 连接虚拟机console
- virsh console ubu0-clone
通过“Ctrl + ]”退出虚拟机

### 通过scp 传输文件
向虚拟机发送文件
- scp -r ./myfile username@192.168.122.112:/home/lyp  
获取虚拟机文件
- scp -r lyp@192.168.122.112:/home/lyp ./



### 修改绑定虚拟机的IP 
- step 1:
查看虚拟机的网卡mac： 如'52:54:00:e9:c6:6a'  
- step 2:
方法1： 通过virsh 修改网络配置  
virsh net-edit default 
方法2： 通过修改文件
sudo vim /etc/libvirt/qemu/networks/default.xml
可以看到以下内容   
```console
<network>
  <name>default</name>
  <uuid>e09babfd-b8f9-47e0-bd97-fb156688e386</uuid>
  <forward mode='nat'/>
  <bridge name='virbr0' stp='on' delay='0'/>
  <mac address='52:54:00:cf:c3:26'/>
  <ip address='192.168.122.1' netmask='255.255.255.0'>
    <dhcp>
      <range start='192.168.122.2' end='192.168.122.254'/>
      <host mac='52:54:00:e9:c6:6a'  name='ubu3' ip='192.168.122.103'/>  //LYP 添加本行，即设置mac地址和ip绑定
    </dhcp>
  </ip>
</network>
```  
- setp 3:  
重启网络和虚拟机  
virsh net-destroy defaule  
virsh net-start defaule  
virsh start ubu3    

- 注意：
如果是通过修改default.xml文件实现修改ip，此时单独执行net-destroy 和 net-start 是无法实现ip设置的，需要执行以下步骤  
1、关闭所有虚拟机（如果虚拟机运行过程中调用了net-destroy，则所有的虚拟机网络无法正常工作）   
2、virsh net-define /etc/libvirt/qemu/networks/default.xml  
3、virsh net-destroy default
4、virsh net-start default  
5、重新启动虚拟机，发现修改的ip已经生效   



### 修改虚拟机的xml  
- virsh edit ubu3



### virsh 创建 macvtap网络   
1、创建虚拟机时选择macvtap网络类型，此时虚拟机xml的网络描述为：  
```xml
<interface type='direct'>
  <mac address='52:54:00:13:95:c0'/>
      <source dev='eno2' mode='bridge'/>
      <model type='virtio'/>
      <address type='pci' domain='0x0000' bus='0x07' slot='0x00' function='0x0'/>
</interface>
```  

2、创建了网络之后，启动虚拟机，如果该网卡没有自动启动， 则：  
```shell
# 临时启动和配置
ifconfig -a #查看该网卡的名称 如 enp7s0 
ifconfig enp7s0 up  #启动该网卡   
ifconfig enp7s0 192.168.0.166  #设置ip  

# 修改该网卡为自动启动
sudo vim /etc/netplan/00xxxxxxx.yaml  
将：
network:
  ethernets:
    enp1s0:
      dhcp4: true
  version: 2
修改为：
network:
  ethernets:
    enp1s0:
      dhcp4: true
    enp7s0:
      dhcp4: true
  version: 2

# 应用该配置  
sudo netplan apply

```


3、通过复制cow2文件和修改xml创建新的虚拟机的步骤   
```md
1、 复制cow2文件和xml文件
2、修改xml文件，具体包括 虚拟机名称、UUID、网络MAC、启动的硬盘路径（改为复制的cow2文件）  
3、定义虚拟机`sudo virsh define new.xml`
4、创建完成，启动虚拟机

注：
如果要删除该虚拟机：`sudo virsh undefine new`
如果虚拟机shutdown关不了，则执行`sudo virsh destroy new`
如果需要编辑虚拟机`sudo virsh edit new`


```



### 通过virt-clone 克隆虚拟机  
`sudo virt-clone --original ubu1 --name ubu3 --auto-clone`

```txt
Options（一些基本的选项）：
--version：查看版本
-h，--help：查看帮助信息
--connect=URI：连接到虚拟机管理程序 libvirt 的URI

General Option（一般选项）：
-o ORIGINAL_GUEST, --original=ORIGINAL_GUEST：原来的虚拟机名称
-n NEW_NAME, --name=NEW_NAME：新的虚拟机名称
--auto-clone：从原来的虚拟机配置自动生成克隆名称和存储路径。
-u NEW_UUID, --uuid=NEW_UUID：克隆虚拟机的新的UUID，默认值是一个随机生成的UUID

Storage Configuration（存储配置）：
-f NEW_DISKFILE, --file=NEW_DISKFILE：指定新的虚拟机磁盘文件
--force-copy=TARGET：强制复制设备
--nonsparse：不使用稀疏文件复制磁盘映像

Networking Configuration:（网络配置）
-m NEW_MAC, --mac=NEW_MAC：设置一个新的mac地址，默认是一个随机的mac
```