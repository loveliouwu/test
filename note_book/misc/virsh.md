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






