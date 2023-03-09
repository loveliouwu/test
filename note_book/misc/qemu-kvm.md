## kvm-qemu 


### kvm-qemu 安装  
sudo apt install kvm-qemu



### qemu 使用  
- 创建一个虚拟机空间
qemu-img create -f qcow2 OS1.img 10G 


- 创建虚拟机  
qemu-system-x86_64 -boot d -cdrom ubuntu-20.04-desktop-amd64.iso -hda OS1.img -m 2048

    qemu-system-x86_64命令是结合自己的电脑构架使用的，因为这边使用的是Intel x64平台；
    boot d：表示从CDROM启动系统，因为虚拟盘中目前还没有系统，需要从CDROM中启动安装盘；
    cdrom：指定了安装的系统镜像；
    hda：指定之前创建的虚拟盘；
    m：指定了QEMU使用的系统内存大小，这里指定的是2G；

	- 快捷键  ctrl+alg+G 切换鼠标窗口




## XML 配置描述  

### os  
- type   
指定了要在虚拟机中启动的操作系统的类型，hvm表示该操作系统是为了在裸机上运行而设计的，rach制定要虚拟化的CPU架构，machine指的是机器类型
```xml
<type arch='x86_64' machine='pc' >hvm</type>  

```


